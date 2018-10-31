#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#define	DEBUG 0
#define SERV_PORT 8099
#define MAX_DATA_SIZE 1024    /*每次最大数据传输量 */
static const int sleep_time = 10;       // 10 sec
static struct hostent *hostname;
static int conn_failures = 0;
static int server_failures = 0;
static int nat_add = 0;
static int nat_del = 0;


struct check_req {
	u_int16_t head; //头部字符
	u_int16_t len_h; //包长度
	u_int16_t len_l; //包长度
	u_int16_t ver; //版本号1.0
	u_int16_t type; //请求类型
	u_int16_t flag;
	u_int16_t checksum;
	u_int16_t reserved1;
	u_int16_t reserved2;
	u_int16_t reserved3;
	u_int16_t tail;
};

struct check_res {
	u_int16_t head; //头部字符
	u_int16_t len_h; //包长度
	u_int16_t len_l; //包长度
	u_int16_t ver; //版本号1.0
	u_int16_t type; //请求类型
	u_int16_t flag;
	u_int16_t checksum;
	u_int16_t reserved1;
	u_int16_t reserved2;
	u_int16_t reserved3;
	u_int8_t mgt_status;
	u_int8_t ser_status;
	u_int16_t tail;
};

void build_req(struct check_req *s_req) {
	unsigned int len = 22;
	s_req->head = htons(0xa7a7);
	s_req->ver = htons(0x0100);
	s_req->type = htons(1000);
	s_req->flag = htons(0);
	s_req->checksum = htons(0);
	s_req->reserved1 = htons(0);
	s_req->reserved2 = htons(0);
	s_req->reserved3 = htons(0);
	s_req->tail = htons(0xb8b8);
	s_req->len_h = htons((len >> 16) & 0xFF);
	s_req->len_l = htons(len & 0xFF);
}

static void millisleep(int ms) {
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms - ts.tv_sec * 1000) * 1000 * 1000;
	nanosleep(&ts, NULL );
}

static int test_programs() {
	int sock_fd, recvbytes;
	char r_buf[MAX_DATA_SIZE];
	char s_buf[MAX_DATA_SIZE];
	struct sockaddr_in serv_addr;
	struct check_req request, t1;
	struct check_res response;
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		if (DEBUG) {
			printf("create socket failed!\n");
		}
		return -1;
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	serv_addr.sin_addr = *((struct in_addr *) hostname->h_addr);
	bzero(&(serv_addr.sin_zero), 8);

	int opt = 1;
	struct timeval tv;
	fd_set set;
	//set non-blocking
	if(ioctl(sock_fd,FIONBIO,&opt)<0){
		close(sock_fd);
		if (DEBUG) {
			printf("ioctl!\n");
		}
		return -1;	
	}

	if(connect(sock_fd,(struct sockaddr *) &serv_addr,
				sizeof(struct sockaddr)) == -1){
		if (errno == EINPROGRESS){
			int error;
			int len = sizeof(int);
			tv.tv_sec = 5;
			tv.tv_usec = 0;
			FD_ZERO(&set);
			FD_SET(sock_fd,&set);
			if(select(sock_fd + 1,NULL,&set,NULL,&tv)>0){
				getsockopt(sock_fd,SOL_SOCKET,SO_ERROR,&error,(socklen_t *)&len);
				if(error != 0){
					close(sock_fd);
					return -1;
				}
			}else{
				close(sock_fd);
				return -1;
			}
		}else{
			close(sock_fd);
			return -1;
		}
	}

	opt = 0;
// set blocking
	if(ioctl(sock_fd,FIONBIO,&opt)<0){
		close(sock_fd);
		return -1;
	}
/*
	if (connect(sock_fd, (struct sockaddr *) &serv_addr,
			sizeof(struct sockaddr)) == -1) {
		if (DEBUG) {
			printf("connect failed!\n");
		}
		return -1;
	}

*/
	build_req(&request);

	memset(s_buf, 0, 1024);
	memcpy(s_buf, &request, sizeof(request));
	memset(&t1, 0, sizeof(t1));
	memcpy(&t1, s_buf, sizeof(t1));
	if (send(sock_fd, s_buf, sizeof(request), 0) == -1) {
		if (DEBUG) {
			printf("send msg failed!\n");
		}
		close(sock_fd);
		return -1;
	}

	memset(r_buf, 0, 1024);
	if ((recvbytes = recv(sock_fd, r_buf, MAX_DATA_SIZE, 0)) == -1) {
		if (DEBUG) {
			printf("recv msg failed!\n");
		}
		close(sock_fd);
		return -1;
	}
	memset(&response, 0, sizeof(response));
	memcpy(&response, r_buf, sizeof(response));
	int type = ntohs(response.type);
	if (type == 1001) {
		if (DEBUG) {
			printf("mgt_staus:%d\n",response.mgt_status);
			printf("ser_staus:%d\n",response.ser_status);
		}
		if (response.mgt_status && response.ser_status) {
			close(sock_fd);
			return 1;
		} else {
			close(sock_fd);
			return 0;
		}
	}
	close(sock_fd);
	return -1;
}

static void natAdd(){
	if(nat_add==0){
		printf("cache up\n");
		system("snd2speed cache up");
		nat_add=1;
		nat_del=0;
	}
}

static void natDel(){
	if(nat_del==0){
		printf("cache down\n");
		system("snd2speed cache down");
		nat_del=1;
		nat_add=0;
	}
}

static void check_programs() {
	int err = 0;
	err = test_programs();
	if (err == -1) {
		conn_failures++;
		if (DEBUG) {
			printf("test_programs failed %d,due to communication problem!\n",
				conn_failures);
		}
		do{
			millisleep(5 * 1000);
			err = test_programs();
			if(err == -1){
				conn_failures++;
				if (DEBUG) {
					printf("test_programs failed %d,due to communication problem!\n",
						conn_failures);
				}
			}else{
				conn_failures = 0;
				break;
			}
		}while(conn_failures<3);
		if (conn_failures >= 3) {
			conn_failures = 0;
			//remove nat
			natDel();
		}

	} else if (err == 0) {
		server_failures++;
		if (DEBUG) {
			printf("test_programs failed %d,due to remote server problem!\n",
					server_failures);
		}
		if (server_failures >= 3) {
			server_failures = 0;
			natDel();
		}
	} else {
		conn_failures = 0;
		server_failures = 0;
		natAdd();
		//if nat not exist,add nat
	}
}

int usage() {
	printf("Usage example:\n");
	printf("cache_check 192.168.1.252 \n");
	return 0;
}

int main(int argc, char *argv[]) {

	if (argc != 2) {
		return usage();
	}

	if ((hostname = gethostbyname(argv[1])) == NULL ) {
		if (DEBUG) {
			printf("host:%s,gethostbyname error!\n", argv[1]);
		}
		return usage();
	}

	for (;;) {
		check_programs();
		millisleep(sleep_time * 1000);
	}

	return 0;
}
