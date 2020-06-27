#include "jmp.h"

/**
 * @fn jmp_t* jmp_init()
 * @brief 프로토콜 메시지 객체를 생성하고 초기화하기 위한 함수
 * @return 프로토콜 메시지 객체
 */
jmp_t* jmp_init(){
	jmp_t* msg = ( jmp_t*)malloc(sizeof( jmp_t));
	memset( msg->data, 0, sizeof( msg->data));
	return msg;
}

/**
 * @fn void jmp_destroy( jmp_t *msg)
 * @brief 프로토콜 메시지 객체를 삭제하기 위한 함수
 * @return void
 * @param msg 삭제할 프로토콜 객체
 */
void jmp_destroy( jmp_t* msg){
	memset( msg->data, 0, jmp_get_data_len( msg));
	free( msg);
}

/**
 * @fn int jmp_get_data_len( jmp_t* msg)
 * @brief 프로토콜 메시지 바디의 데이터 길이를 구하는 함수
 * @return 메시지 바디의 데이터 길이
 * @param msg 소유한 데이터 길이를 알고자하는 메시지 객체
 */
int jmp_get_data_len( jmp_t* msg){
	return strlen( msg->data);
}

/**
 * @fn int jmp_get_data_size( jmp_t* msg)
 * @brief 프로토콜 메시지 바디의 데이터 크기를 구하는 함수
 * @return 메시지 바디의 데이터 크기
 * @param msg 소유한 데이터 크기를 알고자하는 메시지 객체
 */
int jmp_get_data_size(){
	return sizeof( jmp_t) - sizeof( jmp_hd_t);
}

/**
 * @fn char* jmp_get_data( jmp_t* msg)
 * @brief 프로토콜 메시지 바디의 데이터를 구하는 함수
 * @return 메시지 바디의 데이터(문자열)
 * @param msg 소유한 데이터를 알고자하는 메시지 객체
 */
char* jmp_get_data( jmp_t* msg){
	return msg->data;
}

/**
 * @fn int jmp_set_msg( jmp_t *msg, uint32_t seq_id, char *data)
 * @brief 프로토콜 메시지의 헤더와 바디를 지정하는 함수
 * @return 성공 여부(1:success, -1:fail)
 * @param msg 설정하려고 하는 메시지 객체
 * @param seq_id 설정하려고 하는 메시지 헤더의 sequence identifier
 * @param data 설정하려고 하는 메시지 바디 데이터
 */
int jmp_set_msg( jmp_t *msg, uint32_t seq_id, char *data){
	if( data != NULL){
		int len = strlen( data);
		memset( msg->data, 0, DATA_MAX_LEN);
		data[ len] = '\0';
		memcpy( msg->data, data, strlen( data));
		msg->hd.seq_id = seq_id;
		msg->hd.data_len = len;
		return 1;
	}
	return -1;
}


/**
 * @fn void jmp_print_msg( jmp_t *msg)
 * @brief 프로토콜 메시지 전체를 출력하는 함수
 * @return void
 * @param msg 출력하려는 메시지의 객체
 */
void jmp_print_msg( jmp_t *msg){
	if( msg == NULL){
		printf("msg null!\n");
		return ;
	}

	printf("----- msg header -----\n");
	printf("| msg seq id : %d\n", msg->hd.seq_id);
	printf("| msg data len : %d\n", msg->hd.data_len);

	printf("----- msg body -----\n");
	printf("| msg data : %s\n\n", msg->data);
}

