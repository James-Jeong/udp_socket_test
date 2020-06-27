#pragma once

#ifndef __JMP_H__
#define __JMP_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define DATA_MAX_LEN 1024


typedef unsigned short ushort;

/// @struct jmp_hd_t
/// @brief 통신을 위한 프로토콜 헤더 구조체
typedef struct jmp_hd_s jmp_hd_t;
struct jmp_hd_s{
	// message sequence identifier
	uint32_t seq_id;
	// message data length
	int data_len;
};

/// @struct jmp_t
/// @brief 통신을 위한 프로토콜 구조체
typedef struct jmp_s jmp_t;
struct jmp_s{
	// message header
	jmp_hd_t hd;
	// message data
	char data[ DATA_MAX_LEN];
};

jmp_t* jmp_init();
void jmp_destroy( jmp_t *msg);
int jmp_get_data_len( jmp_t *msg);
int jmp_get_data_size();
char* jmp_get_data( jmp_t *msg);
int jmp_set_msg( jmp_t *msg, uint32_t seq_id, char *data);
void jmp_print_msg( jmp_t *msg);

#endif

