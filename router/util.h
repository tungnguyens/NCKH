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

#include "router.h"
#include "my-include.h"

#include <stdio.h>
#include <stdint.h>

#define CBC 1
#define ECB 1

#define DEBUG 0

#if DEBUG /*-----------------------------------*/

#define PRINTF(...)    printf(__VA_ARGS__)

#else /* DEBUG */
#define PRINTF(...)
#endif    /*-----------------------------------*/

#define POLY 0x8408
#define MAX_LEN_CBC 64
#define MAX_LEN_CBC 64

static uint8_t iv[16]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, \
                           0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

typedef enum { false, true } bool;

unsigned short gen_crc16(uint8_t *data_p, unsigned short length);
bool check_crc16(uint8_t *data_p, unsigned short length);

void get_my_adrress(uip_ipaddr_t *ipaddr);
void set_border_router_address(uip_ipaddr_t *ipaddr);
void prepare2send(frame_struct_t *send, uip_ipaddr_t *src_ipaddr, uip_ipaddr_t *dest_ipaddr,\
		 	uint32_t frame_counter, uint8_t state, uint8_t cmd, uint8_t *data);
void 
parse_64(uint8_t* data_64, frame_struct_t *receive);
uint16_t hash(uint16_t a);
void PRINTF_DATA(frame_struct_t *frame);

/*-AES-*/
void decrypt_cbc(uint8_t* data_input, uint8_t* data_output, const uint8_t* key, const uint8_t* iv);
void encrypt_cbc(uint8_t* data_input, uint8_t* data_output, const uint8_t* key, const uint8_t* iv);



#endif /* UTIL_H_ */