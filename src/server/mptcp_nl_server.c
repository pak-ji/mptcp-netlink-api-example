#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>

#include "../hdr/common/std.h"
#include "../hdr/common/libnl_api.h"
#include "../hdr/common/mptcp.h"
#include "../hdr/common/mptcp_nl_api.h"
#include "../hdr/common/mptcp_nl_msg.h"
#include "../hdr/common/cmd_func.h"
#include "../hdr/common/event_func.h"

struct mp_nl_attr	main_attr;
struct mp_nl_attr	tmp_attr;
struct mp_nl_req	mp_nl_req_msg;
struct mp_nl_res	mp_nl_res_msg;

void event_recv_subflow_th(void* arg);

int main(int argc, char** argv)
{
	int mp_main_server_sock;
	int mp_main_client_sock;
	char* addr;
	int port;
	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;
	int client_addr_len;
	char* manager = "netlink";
	int enable = 1;
	char recv_buff[1024] = { '\0', };

	FILE* file;
	int nsize = 0;

	pthread_t thread1;



	if(argc != 3){
		fprintf(stderr, "usage: %s [ip_address] [port_number]\n", argv[0]);
		return -1;
	}
	addr = argv[1];
	port = atoi(argv[2]);

	/* 0. 쓰레드 생성 */
	pthread_create(&thread1, NULL, (void*)event_recv_subflow_th, (void*)addr);
	
	/* 1. 소켓 생성 */
	printf("INFO) Creating socket...\n");
	mp_main_server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(mp_main_server_sock < 0){
		perror("ERROR) socket() ");
		return -1;
	}

	if(setsockopt(mp_main_server_sock, SOL_TCP, MPTCP_ENABLED, &enable, sizeof(enable)) < 0){
		perror("ERROR) setsockopt() ");
		return -1;
	}

	if(setsockopt(mp_main_server_sock, SOL_TCP, MPTCP_PATH_MANAGER, manager, strlen(manager)) < 0){
		perror("ERROR) setsockopt() ");
		return -1;
	}
	printf("INFO) Socket creation completed.\n");

	/* 2. 연결 대기 */
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(addr);
	server_addr.sin_port = htons(port);

	if(bind(mp_main_server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		perror("ERROR) bind() ");
		return -1;
	}

	printf("INFO) Waiting for client connection request.\n");
	if(listen(mp_main_server_sock, 5)){
		perror("ERROR) listen() ");
		close(mp_main_server_sock);
		return -1;
	}

	/* 3. 연결 요청 수락 */
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr_len = sizeof(client_addr);
	mp_main_client_sock = accept(mp_main_server_sock, (struct sockaddr*)&client_addr, &client_addr_len);
	if(mp_main_client_sock < 0){
		perror("ERROR) accept() ");
		close(mp_main_server_sock);
		return -1;
	}

	/* 4. 데이터 수신 */
	file = fopen("./recv_file", "wb");
	if(file == NULL){
		perror("ERROR) fopen() ");
		return -1;
	}

	while(1){
		nsize = recv(mp_main_client_sock, recv_buff, 1024, 0);
		
		if(nsize <= 0) break;
		fwrite(recv_buff, sizeof(char), nsize, file);
	}
	printf("INFO) Received file\n");

	/* End */
	close(mp_main_client_sock);
	close(mp_main_server_sock);
	fclose(file);

	pthread_join(thread1, NULL);

	return 0;
}



void event_recv_subflow_th(void* arg)
{
	char* addr = (char*)arg;

	struct nl_sock* nl_recv_sock;
	struct nl_sock* nl_send_sock;
	char recv_buff[1024];
	int family_id;
	int group_id;
	int ret;

	nl_recv_sock = nl_socket_alloc();
	nl_send_sock = nl_socket_alloc();
	if(!nl_recv_sock || !nl_send_sock){
		fprintf(stderr, "ERROR) Netlink socket allocation error\n");
		return;
	}

	if(genl_connect(nl_recv_sock) || genl_connect(nl_send_sock)){
		fprintf(stderr, "ERROR) Generic netlink connect error\n");
		return;
	}

	family_id = genl_ctrl_resolve(nl_recv_sock, MPTCP_GENL_NAME);
	if(family_id < 0){
		fprintf(stderr, "ERROR) Family id lookup error\n");
		return;
	}

	group_id = genl_ctrl_resolve_grp(nl_recv_sock, MPTCP_GENL_NAME, MPTCP_GENL_EV_GRP_NAME);
	if(group_id < 0){
		fprintf(stderr, "ERROR) Group id lookup error\n");
		return;
	}

	if(nl_socket_add_membership(nl_recv_sock, group_id)){
		fprintf(stderr, "ERROR) Netlink socket group join error\n");
		return;
	}

	while(1){
		memset(recv_buff, 0, sizeof(recv_buff));
		ret = recv(nl_socket_get_fd(nl_recv_sock), recv_buff, sizeof(recv_buff), 0);
		if(ret < 0){
			fprintf(stderr, "ERROR) Event receive error\n");
			return;
		}

		mp_nl_res_msg = extract_event(recv_buff);
		
		switch(mp_nl_res_msg.genlh->cmd){
			case MPTCP_EVENT_CREATED:
				main_attr = event_created(mp_nl_res_msg, 1);

				/* MPTCP_CMD_ANNOUNCE */
				// main
				main_attr.loc_id = 0x3;
				mp_nl_req_msg.req = create_cmd_announce_req(main_attr, family_id);
				nl_send_auto(nl_send_sock, mp_nl_req_msg.req);
				break;

			case MPTCP_EVENT_ESTABLISHED:
				event_established(mp_nl_res_msg, 1);
				break;

			case MPTCP_EVENT_CLOSED:
				event_closed(mp_nl_res_msg, 1);
				pthread_exit(NULL);
				break;

			case MPTCP_EVENT_ANNOUNCED:
				event_announced(mp_nl_res_msg, 1);
				break;

			case MPTCP_EVENT_SUB_ESTABLISHED:
				event_sub_established(mp_nl_res_msg, 1);
				break;

			case MPTCP_EVENT_SUB_PRIORITY:
				event_sub_priority(mp_nl_res_msg, 1);
				break;

			default:
				break;
		}
		printf("\n");
	}
}
