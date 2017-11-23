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
#ifndef END_DEVICE_H_
#define END_DEVICE_H_

#include <stdint.h>

enum port { // port
 			BORDER_ROUTER_LISTEN_PORT 			= 3000,
 			BORDER_ROUTER_LISTEN_PORT_BEGIN 	= 3002,
 			END_DEVICE_LISTEN_PORT				= 3001,
 			ROUTER_LISTEN_PORT 					= 3001,

};

enum state { // state
			STATE_BEGIN 	= 1,
			STATE_NORMAL	= 2,
			STATE_EMERGENCY = 3,
};

enum cmd { 	// begin cmd
			REQUEST_JOIN 	= 1,
			REQUEST_HASH	= 2,
			REPLY_HASH		= 3,
			JOIN_SUCCESS    = 4,
			// normal cmd
			SEND_NORMAL 	= 5,
			// emergency cmd
			SEND_EMERGENCY  = 6,
			RSSI_COLLECT    = 7
};

#define	SFD 		0x7E	/* Start of NCKH frame Delimitter */
#define	MAX_LEN 	64		/* Always 64 */
/*---------------------------------------------------------------------------*/
//	sfd = 0x7E;
//	len = 64;
//	seq: frame counter;
//	type: JOIN/ KEY/ VI_TRI/ NHIP_TIM/ FOR_FUTURE
//	cmd:	command id
//	payload_data[16]: data payload
//  ipv6_source_addr[16] : source address
//	ipv6_dest_addr[16] : destination address
struct frame_struct_t {
	uint8_t  	sfd;
	uint8_t 	len;
	uint32_t 	seq;
	uint8_t		state;
	uint8_t		cmd;
	uint8_t		payload_data[16];
	uint8_t 	ipv6_source_addr[16];
	uint8_t 	ipv6_dest_addr[16];
	uint8_t		for_future[6];
	uint16_t	crc;	
};

typedef struct frame_struct_t	frame_struct_t;

static uint8_t key_begin[16] = "tung_duc_anh_anh";

#endif /* END_DEVICE_H_ */