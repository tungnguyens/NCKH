/*
|-------------------------------------------------------------------|
| HCMC University of Technology                                     |
| Telecommunications Departments                                    |
| Wireless Embedded Firmware for NCKH         						|
| Version: 1.0                                                      |
| Author: ng.sontung.1995@gmail.com									|
| Date: 07/2017                                                     |
| HW support in ISM band: CC2530, SKY								|
| Type device: ROUTER										    	|
|-------------------------------------------------------------------|*/

#define DEBUG DEBUG_PRINT

//#define USING_CC2530	1

#if USING_CC2530
#define MIN_RSSI 100
#else
#define MIN_RSSI 50
#endif

#define TIMEOUT 				15 * CLOCK_SECOND

#include "my-include.h"
#include "router.h"
#include "util.h"
#include "aes.h"

// TCP.IP
#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[uip_l2_l3_hdr_len])

#define MAXBUF	64

static struct uip_udp_conn *router_listen_conn;
static struct uip_udp_conn *router2bor_conn;
static struct uip_udp_conn *router2bor_conn_begin;


static uip_ipaddr_t border_router_ipaddr;
static uip_ipaddr_t my_ipaddr;

// Frame struct
/* frame_struct_t ; */
static frame_struct_t send, send_encrypted;
static frame_struct_t receive, receive_decrypted;
/* frame counter */
static uint32_t frame_counter, counter_udp;

// STATE
static uint8_t state;
static uint8_t begin_cmd;
/**add mew*/
static char buf[MAX_LEN];
static uint16_t len;
static uint8_t 	key_normal[16];
static uint8_t k = 0;

/*---------*/
// My function
static void start_up(void);
static void tcpip_normal_handler(void);
static void send2bor_begin(uint8_t begin_cmd);
static void udp_send(void);
//static void timerout_handler();
/*---------------------------------------------------------------------------*/
/*PROCESS_THREAD */
/*---------------------------------------------------------------------------*/
PROCESS(router_process, "ROUTER PROCESS");
AUTOSTART_PROCESSES(&router_process);
/*---------------------------------------------------------------------------*/
/*MY FUNCTION   */
/*---------------------------------------------------------------------------*/
static void udp_send(void)
{
  uint8_t data[16];
  //uint8_t data_encrypted[16];
  PRINTF("__send2bor_normal_UDP__\n");

  memcpy(&data[0], &key_normal[0], 16);
  memset(&send, 0, sizeof(frame_struct_t));

  prepare2send(&send, &my_ipaddr, &border_router_ipaddr, \
               frame_counter, STATE_NORMAL, SEND_NORMAL, data);//data_encrypted);

  encrypt_cbc((uint8_t *)&send, (uint8_t *)&send_encrypted, \
              (const uint8_t *) key_normal, (uint8_t *)iv);

  uip_udp_packet_send(router2bor_conn, &send_encrypted, MAX_LEN);

  frame_counter ++;
}

/*------------------------------------------------------------------------------*/
static void 
start_up(void){
	PRINTF("__start_up_\n");

	router_listen_conn = udp_new(NULL, UIP_HTONS(0), NULL);
	if(!router_listen_conn) {
	    PRINTF("udp_new router_listen_conn error.\n");
  	}

	udp_bind(router_listen_conn, UIP_HTONS(ROUTER_LISTEN_PORT));
	PRINTF("Listen port: %d, TTL=%u\n", ROUTER_LISTEN_PORT, \
	  										router_listen_conn->ttl);

	PRINTF("My IPV6 address: ");	
	get_my_adrress(&my_ipaddr); 
	PRINT6ADDR(&my_ipaddr);PRINTF("\n");
	

	PRINTF("Border router address: ");
	set_border_router_address(&border_router_ipaddr); //0xaaaa::1
	PRINT6ADDR(&border_router_ipaddr);PRINTF("\n");

	router2bor_conn = udp_new(&border_router_ipaddr, UIP_HTONS(BORDER_ROUTER_LISTEN_PORT), NULL);//r
	if(!router2bor_conn) {
	    PRINTF("udp_new router2bor_conn error.\n");
  	}

  	router2bor_conn_begin = udp_new(&border_router_ipaddr, UIP_HTONS(BORDER_ROUTER_LISTEN_PORT_BEGIN), NULL);//r
	if(!router2bor_conn_begin) {
	    PRINTF("udp_new router2bor_conn_begin error.\n");
  	}

  	// init state
  	state = STATE_BEGIN;
  	begin_cmd = REQUEST_JOIN;

  	frame_counter = 0;

  	PRINTF("__start_up_done_\n");
}
/*---------------------------------------------------------------------------*/
static void 
send2bor_begin(uint8_t begin_cmd){
	uint8_t i;
	uint8_t data[16];	
	PRINTF("__send2bor_begin_\n");
	memset(data, 0, 16);
	switch(begin_cmd){
		case REQUEST_JOIN:
	   			for(i = 0; i<6; i++){
					send.for_future[i] = 0xff;
				}
            	
				prepare2send(&send, &my_ipaddr, &border_router_ipaddr, \
								frame_counter, STATE_BEGIN, REQUEST_JOIN, data);
				//PRINTF_DATA(&send);
				AES128_ECB_encrypt((uint8_t*) &send.payload_data[0], \
						 (const uint8_t*) key_begin, (uint8_t *) &send.payload_data[0]);
 
				uip_udp_packet_send(router2bor_conn, &send, MAX_LEN);
				frame_counter ++;
			break;

		default:
			break;
	}
}
/*---------------------------------------------------------------------------*/
/*ALL HANDLER */
/*---------------------------------------------------------------------------*/
static void
tcpip_begin_handler(void){
	uint16_t hash_a, hash_b;
	uint8_t data[16];
	uint8_t i;
	PRINTF("__Recieve_Msg__\n");
	memset(buf, 0, MAX_LEN);
  	if(uip_newdata()) {
	    len = uip_datalen();
	    memcpy(buf, uip_appdata, MAX_LEN);
	    //PRINTF("chiu dai: %d\r\n", uip_datalen());
	    for(i= 0; i< uip_datalen(); i++){ 
	    	PRINTF("%02x ", (uint8_t) (buf[i]&0xff));
	    	buf[i] = (uint8_t) (buf[i]&0xff);
	    }

        AES128_ECB_decrypt((uint8_t*) &buf[8], (const uint8_t*) key_begin,\
        											(uint8_t *)  &buf[8]);

	    if(check_crc16((uint8_t*) buf, MAX_LEN)){
	    	parse_64((uint8_t*) buf, &receive);
	   		PRINTF_DATA(&receive);
	   		switch (receive.cmd){
	   			case REQUEST_HASH:
	   				begin_cmd = REPLY_HASH;
	   				PRINTF("____receive msg REQUEST_HASH_____");
					hash_a = ((uint16_t)(receive.payload_data[0]) | (uint16_t)(receive.payload_data[1] <<8));
		   			hash_b = hash(hash_a);
		   			data[0] = (uint8_t )(hash_b & 0x00FF);
		   			data[1] = (uint8_t )((hash_b & 0xFF00)>>8);

		   			for(i = 0; i<6; i++){
						send.for_future[i] = 0xff;
					}
		   			prepare2send(&send, &my_ipaddr, &border_router_ipaddr, \
									frame_counter, STATE_BEGIN, REPLY_HASH, data);
	                AES128_ECB_encrypt((uint8_t*) &send.payload_data[0],\
	                	 (const uint8_t*) key_begin, (uint8_t *) &send.payload_data[0]);

		   			uip_udp_packet_send(router2bor_conn_begin, &send, MAX_LEN);
					frame_counter ++;
	   				break;

	   			case JOIN_SUCCESS:
	   				memcpy(&key_normal[0], &receive.payload_data[0], 16);
	   				PRINTF("Key for me:\n");
	                for(i = 0; i<16; i++)
	                    PRINTF("%02x ",key_normal[i]);
	                PRINTF("\r\n");
	   				PRINTF("__Recieve_Msg__JOIN SUCCESS_________");
					state = STATE_NORMAL;
					// k = 0;
	   				break;

	   			default:
	   				break;
	   		}
		}
	}
}
/*---------------------------------------------------------------------------*/

static void
tcpip_normal_handler(void){
	uint8_t data_rev[MAXBUF];
	uint8_t data[16];
	uint8_t i;
	uint16_t rssi, lqi;
	
	memset(buf, 0, MAX_LEN);
	memset(data_rev, 0, MAX_LEN);
	memset(&send, 0, MAX_LEN);
	memset(data, 0, 16);
	
	//printf("__tcpip_normal_handler\n");

    printf("TTL = %d\n", UIP_IP_BUF->ttl);

	rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
	lqi = packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY);
  	printf("RSSI = %d\n",rssi);	 //PRINTF("LQI = %0d\n",lqi);
	
	if(uip_newdata()) {
	    len = uip_datalen();
	    memcpy(buf, uip_appdata, MAX_LEN);
	    for(i= 0; i< uip_datalen(); i++){ 
	    	buf[i] = (uint8_t) (buf[i]&0xff);
	    }
	    
	    memcpy(data_rev, buf, 64);

	    decrypt_cbc((uint8_t *)buf, (uint8_t *)data_rev, \
	    				(const uint8_t *) &key_begin[0], (uint8_t *) iv);

	    if(check_crc16(data_rev, MAXBUF)){  
            parse_64((uint8_t *)data_rev, &receive);
            printf("seq = %ld\n", receive.seq);  //PRINTF_DATA(&receive);
            //printf("BPM = %d\n", receive.payload_data[0]);
 		    
 		    //printf("\n");

            memcpy(data, &receive.payload_data[0], 16);
            data[1] = (uint8_t)(rssi + MIN_RSSI);
            prepare2send(&send, &my_ipaddr, (uip_ipaddr_t*) &receive.ipv6_source_addr[0], receive.seq, STATE_EMERGENCY, SEND_EMERGENCY, data);
            encrypt_cbc((uint8_t *)&send, (uint8_t *)&send_encrypted, \
							(const uint8_t *) key_normal, (uint8_t *)iv);
            uip_udp_packet_send(router2bor_conn, &send_encrypted, MAX_LEN);
        } 		
	}
	printf("----------------------------------------------------------------\n");	
}

/*---------------------------------------------------------------------------*/
/*PROCESS_THREAD */
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(router_process, ev, data) {
	static struct etimer et;
	static uint8_t rand_num;
	/*-----------------------------------------------------------------------*/
	PROCESS_BEGIN();

	start_up();

	etimer_set(&et, TIMEOUT);

	while(state == STATE_BEGIN){		
		if(begin_cmd == REQUEST_JOIN){
			//if( k == 0 )
			leds_toggle(LEDS_GREEN);
			send2bor_begin(REQUEST_JOIN);
		}
		PROCESS_YIELD();
		if(etimer_expired(&et)){				
			etimer_restart(&et);
			begin_cmd = REQUEST_JOIN;
		}
		else if(ev == tcpip_event){
			leds_toggle(LEDS_GREEN);
			tcpip_begin_handler();
		}
	}

	etimer_set(&et, (random_rand()%15+15) * CLOCK_SECOND);
	printf("\nda xong state_begin\n");
	while(state != STATE_BEGIN){
		/* ------ WAIT event  --------*/
    	PROCESS_YIELD();
	    	/* ------ event TCP/IP -------*/
    	if(ev == tcpip_event) { 
      		tcpip_normal_handler();
    	}
    	if (etimer_expired(&et)){
    		// udp_send();
    		etimer_set(&et, (random_rand()%15+15) * CLOCK_SECOND);
    	}
	}

	PROCESS_END();
	/*-----------------------------------------------------------------------*/
}