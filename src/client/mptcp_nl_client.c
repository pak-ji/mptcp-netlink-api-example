/**
 * 
 * @date		xxxx-xx-xx(xxx)
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

void* event_recv_subflow_th(void* arg);
void* toggle_subflow_th(void* arg);

int main(int argc, char** argv)
{
	int mp_main_sock;
	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;
	char tmp_server_addr[15] = { '\0', };
	int tmp_server_port;
	char* manager = "netlink";
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
	char* main_nic_addr, sub_nic_addr;

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

	printf("INFO) You selected number %d NIC\n", main_nic);
	printf("INFO) Main NIC number : %d\n", main_nic);
	printf("INFO) Sub NIC number : %d\n", sub_nic);
	printf("\n");
	
	/* 3. 초기 연결에 사용할 NIC 정보를Client 소켓에 할당 */
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	strtok(nic[main_nic], ":");
	client_addr.sin_addr.s_addr = inet_addr(strtok(NULL , " "));

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
	/*
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
	pthread_create(&thread1, NULL, event_recv_subflow_th, NULL);
	pthread_create(&thread2, NULL, toggle_subflow_th, NULL);
	*/

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
	printf("INFO) Socket creation completed.\n");

	/* 7. 연결 요청 */
	printf("INFO) Connecting to server(%s:%d)...\n", tmp_server_addr, tmp_server_port);
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

	/*
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	*/

	return 0;
}
