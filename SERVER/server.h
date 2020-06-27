#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include "jpool.h"
#include "../PROTOCOL/jmp.h"

#define TH_MAX_LEN 500
#define DATA_MAX_LEN 1024
#define SERVER_PORT 5060

static const size_t thread_num = 10;


/// @struct server_t
/// @brief client 의 요청에 따른 응답을 처리하기 위한 구조체
typedef struct server_s server_t;
struct server_s{
	/// server udp socket file descriptor
	int fd;
	/// server socket address
	struct sockaddr_in addr;
	/// thread pool object
	jpool_t *jpool;
};

/// @struct data_t
/// @brief work 에서 사용할 매개변수 구조체
typedef struct data_s data_t;
struct data_s{
	/// server udp socket file descriptor
	int server_fd;
	/// client sock address
	struct sockaddr_in client_addr;
	/// data from client
	void *arg;
};

server_t* server_init();
void server_destroy( server_t *server);

void server_conn( server_t *server);
void server_process_data( void *data);
void server_wait( server_t *server);

#endif

