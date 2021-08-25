/**
 * @date		2021-08-25(WED)
 * @author		Ji-Hun(INSLAB)
 */

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
#include <ifaddrs.h>

#include "../hdr/common/std.h"
#include "../hdr/common/libnl_api.h"
#include "../hdr/common/mptcp.h"
#include "../hdr/common/mptcp_nl_api.h"
#include "../hdr/common/mptcp_nl_msg.h"
#include "../hdr/common/cmd_func.h"
#include "../hdr/common/event_func.h"

#include "../hdr/client/lookup_nic.h"

struct mp_nl_attr	main_attr;
struct mp_nl_attr	sub_attr;
struct mp_nl_attr	tmp_attr;
struct mp_nl_req	mp_nl_req_msg;
struct mp_nl_res	mp_nl_res_msg;

pthread_mutex_t mutex;
pthread_cond_t cond;

void event_recv_subflow_th(void* arg);
void toggle_subflow_th(void* arg);

int main(int argc, char** argv)
{
	int mp_main_sock;
	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;
	char tmp_server_addr[15] = { '\0', };
	int tmp_server_port;
	char* manager = "netlink";
	char* scheduler = "default";
	int enable = 1;
	char send_buff[1024] = { '\0', };

	FILE* file;
	char* file_path;
	int fsize = 0, nsize = 0;
	int ret;

	char** nic;
	int nic_num;
	int sel_nic1, sel_nic2;
	int main_nic, sub_nic;
	char* main_nic_addr;
	char* sub_nic_addr;

	pthread_t thread1, thread2;




	/* 0. 파일 인자가 정상적으로 입력되었는지 확인 */
	if(argc != 2){
		fprintf(stderr, "usage: %s [file_path]\n", argv[0]);
		return -1;
	}
	file_path = argv[1];
	
	/* 0.1. 존재하는 파일인지 확인 */
	file = fopen(file_path, "rb");
	if(file == NULL){
		fprintf(stderr, "ERROR) Please enter a valid file path.\n");
		return -1;
	}

	/* 1. IP가 부여된 NIC의 개수 구하기 */
	nic = lookup_nic(&nic_num);

	/* 1.1. 만약 NIC 개수가 2개 이상이 아닐 시엔 프로그램 종료 */
	if(nic_num < 2){
		fprintf(stderr, "ERROR) This program must have at least two NICs.\n");
		return -1;
	}

	/* 1.2. 만약 NIC개수가 2개 이상일 시엔 사용할 NIC 2개를 입력 받기 */
	if(nic_num >= 2){
		printf("INFO) Enter two NICs to use.\n");
		printf("----- Available NICs -----\n");
		for(int i=0; i<nic_num; i++){
			printf("%d. %s\n", i, nic[i]); 
		}
		printf("--------------------------\n");
		printf("EX) INPUT >> 0 2\n");
		printf("INPUT >> ");
		scanf("%d %d", &sel_nic1, &sel_nic2);

		if( (sel_nic1 < 0 || sel_nic1 >= nic_num) || 
			(sel_nic2 < 0 || sel_nic2 >= nic_num) ||
			(sel_nic1 == sel_nic2) ){
			fprintf(stderr, "ERROR) Please enter a valid NIC number.\n");
			return -1;
		}
	}

	printf("\n");

	/* 2. 초기 연결에 사용할 NIC 입력 받기 */
	printf("INFO) Select the NIC you want to initially connect.\n");
	printf("----- Selected NICs -----\n");
	if(sel_nic2 > sel_nic1){
		printf("%d. %s\n", sel_nic1, nic[sel_nic1]);
		printf("%d. %s\n", sel_nic2, nic[sel_nic2]);
	}
	else{
		printf("%d. %s\n", sel_nic2, nic[sel_nic2]);
		printf("%d. %s\n", sel_nic1, nic[sel_nic1]);
	}
	printf("-------------------------\n");
	printf("EX) INPUT >> %d\n", sel_nic1);
	printf("INPUT >> ");
	scanf("%d", &main_nic);

	if( (main_nic != sel_nic1) && (main_nic != sel_nic2) ){
		fprintf(stderr, "ERROR) Please enter a valid NIC number.\n");
		return -1;
	}
	if(main_nic == sel_nic1)
		sub_nic = sel_nic2;
	else
		sub_nic = sel_nic1;

	strtok(nic[main_nic], ":");
	main_nic_addr = strtok(NULL, " ");

	strtok(nic[sub_nic], ":");
	sub_nic_addr = strtok(NULL, " ");

	printf("INFO) You selected number %d NIC\n", main_nic);
	printf("INFO) Main NIC(%s) addr : %s\n", strtok(nic[main_nic], ":"), main_nic_addr);
	printf("INFO) Sub NIC(%s) addr : %s\n", strtok(nic[sub_nic], ":"), sub_nic_addr);
	printf("\n");
	
	/* 3. 초기 연결에 사용할 NIC 정보를Client 소켓에 할당 */
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = inet_addr(main_nic_addr);

	/* 4. 연결할 서버의 주소와 포트 입력받기 */
	printf("INFO) Enter the IP address of the server.\n");
	printf("EX) INPUT >> 192.168.1.1\n");
	printf("INPUT >> ");
	scanf("%s", tmp_server_addr);
	printf("\n");
	printf("INFO) Enter the Port number of the server.\n");
	printf("EX) INPUT >> 80\n");
	printf("INPUT >> ");
	scanf("%d", &tmp_server_port);

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(tmp_server_addr);
	server_addr.sin_port = htons(tmp_server_port);

	printf("\n");

	/* 5. 쓰레드 생성 */
	printf("INFO) Creating thread...\n");
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
	pthread_create(&thread1, NULL, (void*)event_recv_subflow_th, (void*)sub_nic_addr);
	pthread_create(&thread2, NULL, (void*)toggle_subflow_th, NULL);
	printf("INFO) Wakeup the event_recv_subflow_th().\n");
	printf("INFO) Sleep the toggle_subflow_th().\n");

	/* 6. 소켓 생성 */
	printf("INFO) Creating socket...\n");
	mp_main_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(mp_main_sock < 0){
		perror("ERROR) socket() ");
		return -1;
	}

	if(setsockopt(mp_main_sock, SOL_TCP, MPTCP_ENABLED, &enable, sizeof(enable)) < 0){
		perror("ERROR) setsockopt() ");
		return -1;
	}

	if(setsockopt(mp_main_sock, SOL_TCP, MPTCP_PATH_MANAGER, manager, strlen(manager)) < 0){
		perror("ERROR) setsockopt() ");
		return -1;
	}

	if(setsockopt(mp_main_sock, SOL_TCP, MPTCP_SCHEDULER, scheduler, strlen(scheduler)) < 0){
		perror("ERROR) setsockopt() ");
		return -1;
	}
	printf("INFO) Socket creation completed.\n");

	/* 7. 연결 요청 */
	printf("INFO) Connecting to server...(%s:%d)\n", tmp_server_addr, tmp_server_port);
	if(bind(mp_main_sock, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0){
		perror("ERROR) bind() ");
		return -1;
	}

	if(connect(mp_main_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		perror("ERROR) connect() ");
		return -1;
	}
	printf("INFO) Completed initial connection with the server.\n");

	/* 8. 데이터 송신 */
	fseek(file, 0, SEEK_END);
	fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	printf("INFO) Sending data to server...(%dBytes)\n", fsize);
	while(nsize!=fsize){
		int fpsize = fread(send_buff, 1, 1024, file);
		nsize += fpsize;
		send(mp_main_sock, send_buff, fpsize, 0);
	}
	printf("INFO) Completed data transfer.\n");

	/* End */
	fclose(file);
	close(mp_main_sock);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	return 0;
}



void event_recv_subflow_th(void* arg)
{
	char* sub_nic_addr = (char*)arg;

	struct nl_sock*	nl_recv_sock;
	struct nl_sock*	nl_send_sock;
	char recv_buff[1024];
	int family_id;
	int group_id;
	int ret;

	nl_recv_sock = nl_socket_alloc();
	nl_send_sock = nl_socket_alloc();
	if(!nl_recv_sock || !nl_send_sock){
		fprintf(stderr, "ERROR) Netlink socket allocation error.\n");
		return;
	}

	if(genl_connect(nl_recv_sock) || genl_connect(nl_send_sock)){
		fprintf(stderr, "ERROR) Generic netlink connect error.\n");
		return;
	}

	family_id = genl_ctrl_resolve(nl_recv_sock, MPTCP_GENL_NAME);
	if(family_id < 0){
		fprintf(stderr, "ERROR) Family id lookup error.\n");
		return;
	}

	group_id = genl_ctrl_resolve_grp(nl_recv_sock, MPTCP_GENL_NAME, MPTCP_GENL_EV_GRP_NAME);
	if(group_id < 0){
		fprintf(stderr, "ERROR) Group id lookup error.\n");
		return;
	}

	if(nl_socket_add_membership(nl_recv_sock, group_id)){
		fprintf(stderr, "ERROR) Netlink socket group join error.\n");
		return;
	}

	while(1){
		memset(recv_buff, 0, sizeof(recv_buff));
		ret = recv(nl_socket_get_fd(nl_recv_sock), recv_buff, sizeof(recv_buff), 0);
		if(ret < 0){
			fprintf(stderr, "ERROR) Event receive error.\n");
			return;
		}

		mp_nl_res_msg = extract_event(recv_buff);

		switch(mp_nl_res_msg.genlh->cmd){
			case MPTCP_EVENT_CREATED:
				main_attr = event_created(mp_nl_res_msg, 1);
				
				/* MPTCP_CMD_ANNOUNCE */
				// main
				main_attr.loc_id = 0x1;
				mp_nl_req_msg.req = create_cmd_announce_req(main_attr, family_id);
				nl_send_auto(nl_send_sock, mp_nl_req_msg.req);

				// sub
				memcpy(&sub_attr, &main_attr, sizeof(struct mp_nl_attr));
				sub_attr.loc_id = 0x2;
				sub_attr.saddr4 = inet_addr(sub_nic_addr);
				mp_nl_req_msg.req = create_cmd_announce_req(sub_attr, family_id);
				nl_send_auto(nl_send_sock, mp_nl_req_msg.req);
				break;

			case MPTCP_EVENT_ESTABLISHED:
				event_established(mp_nl_res_msg, 1);

				/* MPTCP_CMD_SUB_CREATE */
				mp_nl_req_msg.req = create_cmd_sub_create_req(sub_attr, family_id);
				nl_send_auto(nl_send_sock, mp_nl_req_msg.req);
				break;

			case MPTCP_EVENT_CLOSED:
				event_closed(mp_nl_res_msg, 1);
				nl_socket_free(nl_recv_sock);
				pthread_exit(NULL);
				break;

			case MPTCP_EVENT_ANNOUNCED:
				tmp_attr = event_announced(mp_nl_res_msg, 1);
				main_attr.rem_id = tmp_attr.rem_id;
				sub_attr.rem_id = tmp_attr.rem_id;
				break;

			case MPTCP_EVENT_SUB_ESTABLISHED:
				sub_attr = event_sub_established(mp_nl_res_msg, 1);

				/* MPTCP_CMD_SUB_PRIORITY */
				sub_attr.backup = 1;
				mp_nl_req_msg.req = create_cmd_sub_priority_req(sub_attr, family_id);
				nl_send_auto(nl_send_sock, mp_nl_req_msg.req);

				/* wake up toggle_subflow_th() */
				pthread_cond_signal(&cond);
				nl_socket_free(nl_send_sock);
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



void toggle_subflow_th(void* arg)
{
	pthread_cond_wait(&cond, &mutex);

	struct nl_sock*	nl_send_sock;
	int family_id;
	int toggle;
	int ret;
	unsigned char* ptr;

	nl_send_sock = nl_socket_alloc();
	if(!nl_send_sock){
		fprintf(stderr, "ERROR) Netlink socket allocation error.\n");
		return;
	}

	if(genl_connect(nl_send_sock)){
		fprintf(stderr," ERROR) Generic netlink connect error.\n");
		return;
	}

	family_id = genl_ctrl_resolve(nl_send_sock, MPTCP_GENL_NAME);
	if(family_id < 0){
		fprintf(stderr, "ERROR) Family id lookup error.\n");
		return;
	}

	/* 
	 * subflow toggle control (Primary <--> Backup)
	 */
	printf("INFO) The init operation is completed.\n");
	printf("INFO) This program will now proceed the subflow toggle opration.\n");
	while(1){
		memset(&toggle, 0, sizeof(toggle));

		printf("\n");
		printf("----- Current subflow priority status ----- \n");
		ptr = (unsigned char*)&main_attr.saddr4;
		printf("%u.%u.%u.%u:%d ->", 
					*(ptr+0), *(ptr+1), *(ptr+2), *(ptr+3), main_attr.sport);
		ptr = (unsigned char*)&main_attr.daddr4;
		printf("%u.%u.%u.%u:%d = %s\n",
					*(ptr+0), *(ptr+1), *(ptr+2), *(ptr+3), main_attr.dport,
					main_attr.backup?"Backup":"Primary");


		ptr = (unsigned char*)&sub_attr.saddr4;
		printf("%u.%u.%u.%u:%d ->",
					*(ptr+0), *(ptr+1), *(ptr+2), *(ptr+3), sub_attr.sport);
		ptr = (unsigned char*)&sub_attr.daddr4;
		printf("%u.%u.%u.%u:%d = %s\n",
					*(ptr+0), *(ptr+1), *(ptr+2), *(ptr+3), sub_attr.dport,
					sub_attr.backup?"Backup":"Primary");
		printf("------------------------------------------- \n");

		printf("INFO) You want to toggle the priority of the two subflows?\n");
		printf("INFO) Toggle(1) / Toggle Exit(0)\n");
		printf("EX) INPUT >> 1\n");
		printf("INPUT  >> ");
		scanf("%d", &toggle);
		printf("INFO) You can view NIC statistics using \"ifstat\" shell command\n");

		if(toggle == 1){
			if(main_attr.backup){
				main_attr.backup = 0;
				sub_attr.backup = 1;
			}
			else{
				main_attr.backup = 1;
				sub_attr.backup = 0;
			}

			mp_nl_req_msg.req = create_cmd_sub_priority_req(main_attr, family_id);
			nl_send_auto(nl_send_sock, mp_nl_req_msg.req);

			mp_nl_req_msg.req = create_cmd_sub_priority_req(sub_attr, family_id);
			nl_send_auto(nl_send_sock, mp_nl_req_msg.req);
		}
		else if(toggle == 0){
			break;
		}
		else{
			printf("INFO) You entered an invalid number(%d).\n", toggle);
			continue;
		}
	}

	nl_socket_free(nl_send_sock);
	pthread_exit(NULL);
}
