/*
|-----------------------------------------------------------------------------------|
| HCMC University of Technology                                     				|
| Telecommunications Departments                                    				|
| Wireless Embedded Firmware for NCKH         										|
| Version: 1.0                                                      				|
| Author: ng.sontung.1995@gmail.com													|
| Date: 07/2017                                                     				|
| HW support in ISM band: CC2530, SKY												|
| Type device: END DEVICE										    				|
|-----------------------------------------------------------------------------------|*/

#define DEBUG DEBUG_PRINT//0//DEBUG_PRINT

#define USING_CC2530	1

#define TIMEOUT_BEGIN 				15 * CLOCK_SECOND
#define TIMEOUT_NORMAL 				60 * CLOCK_SECOND
#define TIMEOUT_EMERGENCY			 3 * CLOCK_SECOND

#include "my-include.h"
#include "end-device.h"
#include "util.h"
#include "aes.h"

// TCP.IP
#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[uip_l2_l3_hdr_len])

#define MAXBUF	64

static struct uip_udp_conn *end_device_listen_conn;
static struct uip_udp_conn *end2bor_conn;
static struct uip_udp_conn *end2bor_conn_begin;

static struct uip_udp_conn *end2router_conn;


static uip_ipaddr_t border_router_ipaddr;
static uip_ipaddr_t my_ipaddr;
static uip_ipaddr_t router_ipaddr;

// Frame struct
static frame_struct_t send, send_encrypted;
static frame_struct_t receive; //, receive_decrypted;

/* frame counter */
static uint32_t frame_counter;//, counter_udp;

// STATE
static uint8_t state;
static uint8_t begin_cmd;

/**add mew*/
static char buf[MAX_LEN];
static uint16_t len;
static uint8_t 	key_normal[16];


// Cac bien danh cho adc, nhip tim:
#define TAN_SO_LAY_MAU 			0.1
static int16_t adc_value;

const  THRESHOLD = 1220; //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

static uint8_t BPM = 60;
static uint8_t heartbeat=0;


static int16_t count = 0;
static uint8_t a;
static uint8_t k;
//static int sleep;

/*------------------------------------------------------------------------------------------------------------*/
// My function
//static void udp_send(void);
static void udp_send(void);
static void start_up(void);
static void send2bor_begin(uint8_t begin_cmd);

#if USING_CC2530 == 1
static void get_nhip_tim(void);
#endif // USING_CC2530

static void send2bor_BPM(uint8_t BPM);
static void send2router_EMERGENCY(uint8_t BPM);
static void send2bor_emergency(uint8_t BPM);

//static void timerout_handler();
//static void tcpip_normal_handler(void);
/*------------------------------------------------------------------------------------------------------------*/
/*PROCESS_THREAD */
/*------------------------------------------------------------------------------------------------------------*/
PROCESS(end_device_process, "END DEVICE PROCESS");
AUTOSTART_PROCESSES(&end_device_process);
/*------------------------------------------------------------------------------------------------------------*/
/*MY FUNCTION   */
/*------------------------------------------------------------------------------------------------------------*/
/*
static void 
udp_send(void)
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

  uip_udp_packet_send(end2bor_conn, &send_encrypted, MAX_LEN);

  frame_counter ++;
}
*/
/*--------------------------------------------------------------------------------------------------------------*/
#if USING_CC2530 == 1
static void get_nhip_tim(void)
{
	count++;
	if (count < 150) {
		leds_on(LEDS_GREEN);
		leds_off(LEDS_RED);
		adc_value = read_adc(0x07);

		if (adc_value < THRESHOLD) {
			k=0;
			a=0;
			//leds_off(LEDS_RED);
		}

		if ((adc_value > THRESHOLD) && (a==0)) {			
			k++;
		}

		if ((adc_value > THRESHOLD) && (a==0) && (k==1)) {
			leds_on(LEDS_RED);
			a++;
			heartbeat++;
			k=0;
			printf("hearbeat detected\n");
		}
	}

	if (count == 150) {
		BPM = (BPM + (heartbeat*4))/2;
		printf("-------------\n");
		printf("| BPM = %d |\n", BPM);
		printf("-------------\n");
		heartbeat = 0;
		leds_off(LEDS_GREEN);
	}
	
	if (count > 150) {
		leds_off(LEDS_GREEN); 			
		leds_off(LEDS_RED);
	}

	if (count >= 300) count = 0;
	printf("count = %d\n",count);
	//printf("ADC value = %d\n",adc_value);
}
#endif /* USING_CC2530 */
/*------------------------------------------------------------------------------------------------------------*/
static void 
start_up(void)
{
	PRINTF("__start_up_\n");

	end_device_listen_conn = udp_new(NULL, UIP_HTONS(0), NULL);
	if(!end_device_listen_conn) {
	    PRINTF("udp_new end_device_listen_conn error.\n");
  	}

	udp_bind(end_device_listen_conn, UIP_HTONS(END_DEVICE_LISTEN_PORT));
	PRINTF("Listen port: %d, TTL=%u\n", END_DEVICE_LISTEN_PORT, \
	  										end_device_listen_conn->ttl);

	PRINTF("My IPV6 address: ");	
	get_my_adrress(&my_ipaddr); 
	PRINT6ADDR(&my_ipaddr);PRINTF("\n");
	

	PRINTF("Border router address: ");
	set_border_router_address(&border_router_ipaddr); //0xaaaa::1
	PRINT6ADDR(&border_router_ipaddr);PRINTF("\n");

	end2bor_conn = udp_new(&border_router_ipaddr, UIP_HTONS(BORDER_ROUTER_LISTEN_PORT), NULL);//r
	if(!end2bor_conn) {
	    PRINTF("udp_new end2bor_conn error.\n");
  	}

  	end2bor_conn_begin = udp_new(&border_router_ipaddr, UIP_HTONS(BORDER_ROUTER_LISTEN_PORT_BEGIN), NULL);//r
	if(!end2bor_conn_begin) {
	    PRINTF("udp_new end2bor_conn_begin error.\n");
  	}

  	// EMERGENCY
  	uip_ip6addr(&router_ipaddr, \
				0xFF02,0x0000,0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0002); // link local multicast address
	PRINTF("Link local multicast: ")			
	PRINT6ADDR(&router_ipaddr); PRINTF("\n");
	end2router_conn = udp_new(&router_ipaddr, UIP_HTONS(ROUTER_LISTEN_PORT), NULL);
  	if(!end2router_conn){
	    PRINTF("udp_new end2router_conn error.\n");
  	}

  	// init state
  	state = STATE_BEGIN;
  	begin_cmd = REQUEST_JOIN;

  	frame_counter = 0;
  	count = 0

  	PRINTF("__start_up_done_\n");
}
/*------------------------------------------------------------------------------------------------------------*/
static void 
send2bor_begin(uint8_t begin_cmd)
{
	uint8_t i;
	uint8_t data[16];	
	PRINTF("__send2bor_begin_\n");
	memset(data, 0, 16);
	switch(begin_cmd){
		case REQUEST_JOIN:
	   			for(i = 0; i<6; i++){
					send.for_future[i] = 0xff;
				}
            	
				prepare2send(&send, &my_ipaddr, &router_ipaddr, \
								frame_counter, STATE_BEGIN, REQUEST_JOIN, data);
				//PRINTF_DATA(&send);
				AES128_ECB_encrypt((uint8_t*) &send.payload_data[0], \
						 (const uint8_t*) key_begin, (uint8_t *) &send.payload_data[0]);
 
				uip_udp_packet_send(end2bor_conn, &send, MAX_LEN);
				frame_counter ++;
			break;

		default:
			break;
	}
}

/*--------------------------------------------------------------------------------------------------------------*/

static void send2bor_BPM(uint8_t BPM)
{
	uint8_t data[16];
	memset(&data[0], 0, 16);
  	//uint8_t data_encrypted[16];

  	PRINTF("__send2bor_BPM_=%d\n", BPM);
	data[0] = BPM;

  	memset(&send, 0, sizeof(frame_struct_t));

  	prepare2send(&send, &my_ipaddr, &border_router_ipaddr, \
               frame_counter, STATE_NORMAL, SEND_NORMAL, data);

  	encrypt_cbc((uint8_t *)&send, (uint8_t *)&send_encrypted, \
              (const uint8_t *) key_normal, (uint8_t *)iv);

  	uip_udp_packet_send(end2bor_conn, &send_encrypted, MAX_LEN);

  	frame_counter ++;
}

/*--------------------------------------------------------------------------------------------------------------*/
static void send2router_EMERGENCY(uint8_t BPM){
	uint8_t i;

	uint8_t data[16];
	memset(&data[0], 0, 16);
	printf("__send2router__BPM=%d", BPM);

	data[0] = BPM;
	memset(&send, 0, sizeof(frame_struct_t));

	prepare2send(&send, &my_ipaddr, &border_router_ipaddr, \
               frame_counter, STATE_EMERGENCY, SEND_NORMAL, data);

	encrypt_cbc((uint8_t *)&send, (uint8_t *)&send_encrypted, \
              (const uint8_t *) key_begin, (uint8_t *)iv);

	uip_udp_packet_send(end2router_conn, &send_encrypted, MAX_LEN);
	
	frame_counter ++;	
}

/*--------------------------------------------------------------------------------------------------------------*/
static void send2bor_emergency(uint8_t BPM)
{
	uint8_t data[16];
	memset(&data[0], 0, 16);
	printf("__send2bor_emergency__BPM=%d", BPM);

	data[0] = BPM;
	memset(&send, 0, sizeof(frame_struct_t));

	prepare2send(&send, &my_ipaddr, &border_router_ipaddr, \
               frame_counter, STATE_EMERGENCY, RSSI_COLLECT, data);
	encrypt_cbc((uint8_t *)&send, (uint8_t *)&send_encrypted, \
              (const uint8_t *) key_normal, (uint8_t *)iv);

	uip_udp_packet_send(end2bor_conn, &send_encrypted, MAX_LEN);

	frame_counter ++;
}
/*--------------------------------------------------------------------------------------------------------------*/
/*ALL HANDLER */
/*--------------------------------------------------------------------------------------------------------------*/
static void
tcpip_begin_handler(void)
{
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

		   			uip_udp_packet_send(end2bor_conn_begin, &send, MAX_LEN);
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
	   				break;

	   			default:
	   				break;
	   		}
		}
	}
}
/*--------------------------------------------------------------------------------------------------------------*/

// static void
// tcpip_normal_handler(void)
// {

// }

/*--------------------------------------------------------------------------------------------------------------*/
/*PROCESS_THREAD */
/*--------------------------------------------------------------------------------------------------------------*/
PROCESS_THREAD(end_device_process, ev, data) 
{
	static struct etimer et, et2, et3;
	static uint8_t rand_num;
	static uint8_t tung = 0;
/*--------------------------------------------------------------------------------------------------------------*/
	PROCESS_BEGIN();

	start_up();

	etimer_set(&et, TIMEOUT_BEGIN);

	while(state == STATE_BEGIN){		
		if(begin_cmd == REQUEST_JOIN){
			//if( k == 0 )
			printf("send REQUEST_JOIN\n");
			leds_toggle(LEDS_RED);
			send2bor_begin(REQUEST_JOIN);
		}
		PROCESS_YIELD();
		if(etimer_expired(&et)){				
			etimer_restart(&et);
			begin_cmd = REQUEST_JOIN;
		}
		else if(ev == tcpip_event){
			leds_toggle(LEDS_RED);
			tcpip_begin_handler();
		}
	}

	

	printf("\nda xong state_begin\n");

	leds_on(LEDS_GREEN);

#if USING_CC2530 == 1
	etimer_set(&et, TAN_SO_LAY_MAU * CLOCK_SECOND);
	etimer_set(&et2, TIMEOUT_NORMAL);
	

	while(state != STATE_BEGIN){
		PROCESS_YIELD();
		if(etimer_expired(&et)){
			//printf("get nhip tim\n");			
			get_nhip_tim();			
			etimer_restart(&et);
		}
		if(etimer_expired(&et2)){
			printf("---------------\nTIMEOUT_NORMALs\n---------------\n");
			if(BPM > 60 && BPM < 90){
				printf("Nhip tim OK - send BPM to border\n");
				send2bor_BPM(BPM);
			}else {
				printf("Nhip tim NOT_OK **** EMERGENCY ****\n");
				printf("send multicast link local\n");
				if(tung == 0){
					send2router_EMERGENCY(BPM);
					tung = 1;
					etimer_set(&et2, TIMEOUT_EMERGENCY);

				}else{
				    send2bor_emergency(BPM);
				    tung = 0;
				    etimer_set(&et2, TIMEOUT_NORMAL);
				}

			}			
			etimer_restart(&et2);
		}
	}

#else // USING_CC2530
	//etimer_set(&et, TAN_SO_LAY_MAU * CLOCK_SECOND);
	etimer_set(&et, TIMEOUT_NORMAL);
	SENSORS_ACTIVATE(button_sensor); // << button emergency cooja
	//tung = 1;
	while(state != STATE_BEGIN){
		PROCESS_YIELD();

		// if(etimer_expired(&et)){
		// 	//...
		// 	etimer_restart(&et);
		// }

		if(etimer_expired(&et)){
			printf("---------------\nTIMEOUT_NORMALs\n---------------\n");
			if(tung == 1){
				BPM = 1;
			    send2bor_emergency(BPM);
			    tung = 0;
			    etimer_set(&et, TIMEOUT_NORMAL);
			} else{
				BPM = random_rand()%20 + 60;
				send2bor_BPM(BPM);					
				etimer_restart(&et);

			}
		}

		//if(ev == sensors_event && data == &button_sensor) {
		if(data == &button_sensor) {
      		printf("button clicked, sending emergency\n");
      		etimer_stop(&et);
      		BPM = 1;
      		if(tung == 0){
				printf("Nhip tim NOT_OK **** EMERGENCY ****\n");
				printf("send multicast link local\n");
				send2router_EMERGENCY(BPM);
				tung = 1;
				etimer_set(&et, TIMEOUT_EMERGENCY);

			}
		}
	 }
#endif // USING_CC2530


	PROCESS_END();
/*--------------------------------------------------------------------------------------------------------------*/
}