/*
|-------------------------------------------------------------------|
| HCMC University of Technology                                     |
| Telecommunications Departments                                    |
| Wireless Embedded Firmware for NCKH         						|
| Version: 1.0                                                      |
| Author: ng.sontung.1995@gmail.com									|
| Date: 07/2017                                                     |
| HW support in ISM band: CC2530, SKY								|
|-------------------------------------------------------------------|*/
#ifndef UTIL_H
#define UTIL_H_

#include "test.h"
#include "my-include.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef enum { false, true } bool;

#define DEBUG 1
#if DEBUG
#define PRINTF(...)    printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define LEN_CHAR_IPV6       26
#define ACCEPT				1
#define	DENY				0

#define POLY 				0x8408

#define CBC 				1
#define ECB 				1
#define MAX_LEN_CBC 		64
//#define MAX_DEVICE			8


/*---define finding position --*/
// #define USING_EUCLID  		0
// #define USING_KNN     		0
// #define USING_BAYES   		1

#define NUMBER_K     		11
#define COLUM_MAX 			8// SO ROUTER
#define ROW_MAX   			625// SO MAU CAN LAY
#define MAX_POSITION    	8
#define MAX             	1
#define MIN             	2

#define MAX_FEATURE       90



static uint8_t iv[16]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, \
                           0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

static uint8_t key_begin[16] = "tung_duc_anh_anh";

extern uint8_t end_device_pos,router_pos;
extern node_t 			end_device[MAX_END_DEVICE + 1];
extern node_t 			router[MAX_ROUTER + 1];
extern node_t			node_alt;
extern node_t 			my_device[MAX_DEVICE +1 ];

unsigned short gen_crc16(uint8_t *data_p, unsigned short length);
bool check_crc16(uint8_t *data_p, unsigned short length);
void phex_8(uint8_t* data_8);
void phex_64(uint8_t* data_64);
void parse_64(uint8_t* data_64, frame_struct_t *receive);
//***util.c
void ipv6_to_str_unexpanded(char * str, const struct in6_addr * addr);
uint16_t hash(uint16_t a);
uint16_t gen_random_num();
void gen_random_key_128(unsigned char* key);
void
prepare2send(frame_struct_t *send, uint8_t *src_ipaddr, uint8_t *dest_ipaddr,\
            uint32_t frame_counter, uint8_t state, uint8_t cmd, uint8_t *data);
void copy_node2node(node_t *node_scr, node_t *node_des);
void PRINTF_DATA_NODE(node_t *node_id);
void PRINTF_DATA(frame_struct_t *frame);
void PRINT_ALL();

/*-AES-*/
void decrypt_cbc(uint8_t* data_input, uint8_t* data_output, const uint8_t* key, const uint8_t* iv);
void encrypt_cbc(uint8_t* data_input, uint8_t* data_output, const uint8_t* key, const uint8_t* iv);

void my_memcpy(uint8_t *dest, uint8_t *src, int len);
uint8_t check_router_list(uint8_t *router_ipaddr);
uint8_t check_end_device_list(uint8_t *end_device_ipaddr);
bool checklist(uint8_t *device_ipaddr);
bool check_statedata_rev (char *rev_buffer); //hoang anh
bool check_seq(uint32_t new_seq,uint32_t old_seq); //hoang anh

struct room{
    double room[MAX_POSITION][ROW_MAX][COLUM_MAX];
};
typedef struct room room;

struct signal{
double sig[6][COLUM_MAX];
//double ze2one[COLUM_MAX];
//double one2two[COLUM_MAX];
//double two2three[COLUM_MAX];
//double three2for[COLUM_MAX];
//double for2five[COLUM_MAX];
//double five2six[COLUM_MAX];
//double six2seven[COLUM_MAX];
//double seven2eigth[COLUM_MAX];
}; 
typedef struct signal signal;


/*--------------------------*/
void update_data(room *des,double source[][ROW_MAX][COLUM_MAX], uint8_t length);
void sort_increase(double a[],uint32_t length);
uint8_t min_max_value(double data[],uint32_t length,uint8_t type);
uint8_t locate_KNN(double datainput[],room *data_cp, uint8_t k );
uint8_t locate_euclid(double datainput[],room *data_cp);
uint8_t bayes_locate( double datainput[],room *data_cp );

void convert_hex2str(uint8_t *hex, char *str);

#endif /* UTIL_H_ */