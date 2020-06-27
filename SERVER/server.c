#include "server.h"

/**
 * @mainpage Project : UDP Socket test
 * @section intro 소개
 *    - 차후 계획한 개인 프로젝트를 위해 미리 제작 
 * @section  CreateInfo 작성 정보
 *    - 작성자 :   정동욱
 *    - 작성일 :   2020/06/27
 * @subsection exec 실행 방법 및 인수 설명
 *    - 실행 방법\n
 *      서버       : SERVER/server
 *      클라이언트 : CLIENT/client
 */

/**
 * @fn int main( int argc, char **argv)
 * @brief server 구동을 위한 main 함수
 * @return int
 * @param argc 매개변수 개수
 * @param argv ip 와 포트 정보
 */
int main( int argc, char **argv){
	//	if( argc != 3){
	//		printf("	| ! need param : ip port\n");
	//		return -1;
	//	}

	server_t *server = server_init();
	if( server == NULL){
		printf("	| ! Server : Fail to initialize\n");
		return -1;
	}

	server_conn( server);
	server_wait( server);

	server_destroy( server);
}



// -----------------------------------------

/**
 * @fn static void data_init( data_t *data, server_t *server, struct sockaddr_in client_addr, void *arg)
 * @brief work 에 사용될 함수의 매개변수를 구성하기 위한 구조체를 초기화하는 함수
 * @return void
 * @param data 매개변수를 구성하기 위한 구조체
 * @param server udp socket fd 를 사용하기 위한 server 객체
 * @param client_addr 데이터 처리 후 결과 데이터를 특정 client 로 보내기 위한 주소
 * @param arg client 로 부터 수신한 데이터
 */
static void data_init( data_t *data, server_t *server, struct sockaddr_in client_addr, void *arg){
	data->server_fd = server->fd;
	data->client_addr = client_addr;
	data->arg = arg;
}

/**
 * @fn server_t* server_init()
 * @brief server 객체를 생성하고 초기화하는 함수
 * @return 생성된 server 객체
 */
server_t* server_init(){
	server_t *server = ( server_t*)malloc( sizeof( server_t));

	memset( &server->addr, 0, sizeof( struct sockaddr));
	server->addr.sin_family = AF_INET;
	server->addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server->addr.sin_port = htons(SERVER_PORT);

	if( ( server->fd = socket( PF_INET, SOCK_DGRAM, 0)) < 0){
		printf("	| ! Server : Fail to open socket\n");
		free( server);
		return NULL;
	}

	if( bind( server->fd, ( struct sockaddr*)( &server->addr), sizeof( server->addr)) < 0){
		printf("	| ! Server : Fail to bind socket\n");
		close( server->fd);
		free( server);
		return NULL;
	}

	server->jpool = jpool_init( thread_num);
	if( server->jpool == NULL){
		close( server->fd);
		free( server);
		return NULL;
	}

	printf("	| @ Server : Success to create a object\n");
	printf("	| @ Server : Welcome\n\n");
	return server;
}

/**
 * @fn void server_destroy( server_t *server)
 * @brief server 객체를 삭제하기 위한 함수
 * @return void
 * @param server 삭제하기 위한 server 객체
 */
void server_destroy( server_t *server){
	jpool_destroy( server->jpool);
	close( server->fd);
	free( server);
	printf("	| @ Server : Success to destroy the object\n");
	printf("	| @ Server : BYE\n\n");
}

/**
 * @fn void server_conn( server_t *server)
 * @brief client 와 연결되었을 때 데이터를 수신하고 데이터를 처리하기 위한 함수
 * @return void
 * @param server 데이터 처리를 위한 server 객체
 */
void server_conn( server_t *server){
	if( server->fd <= 0){
		printf("	| ! Server : fd error\n");
		return;
	}

	struct sockaddr_in client_addr;
	int addr_len = sizeof( struct sockaddr);
	ssize_t recv_bytes;
	memset( &client_addr, 0, addr_len);

	while(1){
		printf("        | @ Server : waiting...\n");
		int client_fd;
		char read_buf[ DATA_MAX_LEN];

		jmp_t recv_msg;
		if( ( recv_bytes = recvfrom( server->fd, &recv_msg, sizeof( recv_msg), 0, ( struct sockaddr*)( &client_addr), &addr_len)) <= 0){
			printf("	| ! Server : Fail to recv msg\n\n");
		}
		else{
			jmp_print_msg( &recv_msg);
			snprintf( read_buf, DATA_MAX_LEN, "%s", jmp_get_data( &recv_msg));

			char *client_ip = inet_ntoa( client_addr.sin_addr);
			printf("	| @ Server : [ %s ] > %s (%lu bytes)\n", client_ip, read_buf, recv_bytes);

			if( !memcmp( read_buf, "q", 1)){
				printf("	| @ Server : Finish\n");
				break;
			}

			data_t data;
			data_init( &data, server, client_addr, ( void*)( read_buf));
			jpool_add_work( server->jpool, server_process_data, &data);
		}
	}
}

/**
 * @fn void server_process_data( void* data)
 * @brief work 에서 사용될 데이터 처리 함수
 * @return void
 * @param data client 로 부터 수신한 데이터
 */
void server_process_data( void* data){
	struct sockaddr_in client_addr = ( struct sockaddr_in)( ( ( data_t*)( data))->client_addr);
	int server_fd = ( int)( ( ( data_t*)( data))->server_fd);

	ssize_t send_bytes;
	char send_buf[ DATA_MAX_LEN];
	snprintf( send_buf, DATA_MAX_LEN, "%s", "OK");

	jmp_t send_msg;
	jmp_set_msg( &send_msg, 0, send_buf);

	if( ( send_bytes = sendto( server_fd, &send_msg, sizeof( send_msg), 0, ( struct sockaddr*)( &client_addr), sizeof( client_addr))) <= 0){
		printf("	| ! Server : Fail to send msg\n");
		printf("	| ! Server data : [ %s ]\n", send_buf);
		printf("	| ! Client ip : [ %s ]\n\n", inet_ntoa( client_addr.sin_addr));
	}
}

/**
 * @fn void server_wait( server_t *server)
 * @brief 동작 중인 thread 가 있는지 확인하는 함수
 * @return void
 * @param server 확인에 사용될 server 객체
 */
void server_wait( server_t *server){
	jpool_wait( server->jpool);
}

