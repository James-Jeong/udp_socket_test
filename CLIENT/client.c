#include "client.h"

/**
 * @fn int main( int argc, char **argv)
 * @brief client 구동을 위한 main 함수
 * @return int
 * @param argc 매개변수 개수
 * @param argv ip 와 포트 정보
 */
int main( int argc, char **argv){
	//	if( argc != 3){
	//		printf("	| ! need param : ip port\n");
	//		return -1;
	//	}

	client_t *client = client_init();
	if(client == NULL) return -1;

	client_process_data( client);

	client_destroy( &client);
}



// -----------------------------------------

/**
 * @fn client_t* client_init()
 * @brief server 객체를 생성하고 초기화하는 함수
 * @return 생성된 server 객체
 */
client_t* client_init(){
	client_t *client = ( client_t*)( malloc( sizeof( client_t)));
	if(client == NULL) return NULL;

	memset( &client->server_addr, 0, sizeof( client->server_addr));
	client->server_addr.sin_family = AF_INET;
	inet_aton( server_ip, (struct in_addr*)(&client->server_addr.sin_addr.s_addr));
	client->server_addr.sin_port = htons(SERVER_PORT);

	if( ( client->fd = socket( AF_INET, SOCK_DGRAM, 0)) == -1){
		printf("        | ! Client : Fail to open socket\n");
		return NULL;
	}

	client->seq_id = 0;

	printf("        | @ Client : Success to create a object\n");
	printf("        | @ Client : Welcome\n\n");
	return client;
}

/**
 * @fn void client_destroy( client_t **client)
 * @brief client 객체를 삭제하기 위한 함수
 * @return void
 * @param client 삭제하기 위한 client 객체
 */
void client_destroy( client_t **client){
	if(*client == NULL) return NULL;
	close( (*client)->fd);
	free( *client);
	*client = NULL;
	printf("        | @ Client : Success to destroy the object\n");
	printf("        | @ Client : BYE\n\n");
}

/**
 * @fn void client_process_data( client_t *client)
 * @brief server 로 요청을 보내서 응답을 받는 함수
 * @return void
 * @param client 요청을 하기 위한 client 객체
 */
void client_process_data( client_t *client){
	if(client == NULL) return NULL;
	
	int server_addr_size = 0;
	char read_buf[ DATA_MAX_LEN];
	char send_buf[ DATA_MAX_LEN];
	ssize_t recv_bytes, send_bytes;

	printf("        | @ Client : put the data\n");
	while(1){
		server_addr_size = sizeof( client->server_addr);
		char msg[ DATA_MAX_LEN];
		printf("        | @ Client : > ");
		fgets( msg, DATA_MAX_LEN, stdin);
		msg[ strlen( msg) - 1] = '\0';
		snprintf( send_buf, DATA_MAX_LEN, "%s", msg);

		client->seq_id++;

		jmp_t send_msg;
		jmp_set_msg( &send_msg, client->seq_id, send_buf);
		if( ( send_bytes = sendto( client->fd, &send_msg, sizeof( send_msg), 0, ( struct sockaddr*)( &client->server_addr), sizeof( client->server_addr))) <= 0){
			printf("        | ! Client : Fail to send msg\n");
			printf("        | ! Client data : [ %s ]\n\n", send_buf);
		}
		else{
			if( !memcmp( send_buf, "q", 1)){
				printf("        | @ Client : Finish\n");
				break;
			}

			jmp_t recv_msg;
			if( ( recv_bytes = recvfrom( client->fd, &recv_msg, sizeof( recv_msg), 0, ( struct sockaddr*)( &client->server_addr), &server_addr_size)) <= 0){
				printf("        | ! Client : Fail to recv msg\n\n");
			}
			else{
				jmp_print_msg( &recv_msg);
				snprintf( read_buf, DATA_MAX_LEN, "%s", jmp_get_data( &recv_msg));
				printf("        | @ Client : [ %s ] > %s (%lu bytes)\n", server_ip, read_buf, recv_bytes);
			}
		}
		printf("\n");
	}
}

