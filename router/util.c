#include "util.h"
#include "aes.h"

/*---------------------------------------------------------------------------*/
unsigned short 
gen_crc16(uint8_t *data_p, unsigned short length)
{
    unsigned char i;
    unsigned int data;
    unsigned int crc = 0xffff;

    if (length == 0)
          return (~crc);

    do
    {
          for (i=0, data=(unsigned int)0xff & *data_p++;
               i < 8; 
               i++, data >>= 1)
          {
                if ((crc & 0x0001) ^ (data & 0x0001))
                      crc = (crc >> 1) ^ POLY;
                else  crc >>= 1;
          }
    } while (--length);

    crc = ~crc;
    data = crc;
    crc = (crc << 8) | (data >> 8 & 0xff);

    return (crc);
}


/*---------------------------------------------------------------------------*/
bool check_crc16(uint8_t *data_p, unsigned short length)
{
    unsigned short crc16_check;
    uint8_t c0, c1;
    c0 = c1 = 0;
    crc16_check = gen_crc16(data_p, length - 2);
    c0 = (uint8_t)(crc16_check & 0x00ff);
    c1 = (uint8_t)((crc16_check & 0xff00) >> 8);

    if( c0 == data_p[length -2] && c1 == data_p[length -1]){
        return true;
    }
    else return false;
}
/*---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
void 
get_my_adrress(uip_ipaddr_t *ipaddr) {
	uint16_t ip[4];

	ip[0] = ((uint16_t)rimeaddr_node_addr.u8[0])<<8|(uint16_t)rimeaddr_node_addr.u8[1];
	ip[1] = ((uint16_t)rimeaddr_node_addr.u8[2])<<8|(uint16_t)rimeaddr_node_addr.u8[3];
	ip[2] = ((uint16_t)rimeaddr_node_addr.u8[4])<<8|(uint16_t)rimeaddr_node_addr.u8[5];
	ip[3] = ((uint16_t)rimeaddr_node_addr.u8[6])<<8|(uint16_t)rimeaddr_node_addr.u8[7];
	uip_ip6addr(ipaddr, 0xaaaa, 0, 0, 0, 0x0212, ip[1], ip[2], ip[3]);
}
/*-----------------------------------------------------------------------------------*/
void 
set_border_router_address(uip_ipaddr_t *ipaddr) {
  // change this IP address depending on the node that runs the server!
  uip_ip6addr(ipaddr, 0xaaaa,0x0000,0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0001);
}
/*-----------------------------------------------------------------------------------*/
void
prepare2send(frame_struct_t *send, uip_ipaddr_t *src_ipaddr, uip_ipaddr_t *dest_ipaddr,\
		 	uint32_t frame_counter, uint8_t state, uint8_t cmd, uint8_t *data){

	send->sfd = SFD;
	send->len = MAX_LEN;
	send->seq = frame_counter;
	send->state = state;
	send->cmd = cmd;

	memcpy(send->payload_data, data, 16);
	memcpy(send->ipv6_source_addr, src_ipaddr->u8, 16);
	memcpy(send->ipv6_dest_addr, dest_ipaddr->u8, 16);
	//memset(send->for_future, 0, 6);

	send->crc = gen_crc16((uint8_t *)send, MAX_LEN - 2);

	PRINTF_DATA(send);

}

/*---------------------------------------------------------------------------*/
void 
parse_64(uint8_t* data_64, frame_struct_t *receive) 
{ // in chuoi hex 64 bytes
    int j;
    unsigned char i = 0;
    uint32_t b0, b1, b2, b3;
    uint16_t c0, c1;
    b0 = b1 = b2 = b3 = 0;
    c0 = c1 = 0;

    receive->sfd = *(data_64 + i++);
    receive->len = *(data_64 + i++);

    b0 = (uint32_t) *(data_64 + i++);
    receive->seq = b0;
    b1 = ((uint32_t) *(data_64 + i++)) << 8;
    receive->seq += b1;
    b2 = ((uint32_t) *(data_64 + i++)) << 16;
    receive->seq += b2;
    b1 = ((uint32_t) *(data_64 + i++)) << 24;
    receive->seq += b3;

    receive->state = *(data_64 + i++);
    receive->cmd = *(data_64 + i++);

    for(j=0; j<16; j++){
        receive->payload_data[j] = *(data_64 + i++);
    }

    for(j=0; j<16; j++){
        receive->ipv6_source_addr[j] = *(data_64 + i++);
    }

    for(j=0; j<16; j++){
        receive->ipv6_dest_addr[j] = *(data_64 + i++);
    }

    for(j=0; j<6; j++){
        receive->for_future[j] = *(data_64 + i++);
    }

    c0 = (uint16_t) *(data_64 + i++);
    c1 = (uint16_t) *(data_64 + i++) << 8;

    receive->crc = c0 + c1;
}
/*--------------------------------------------------------------------------------*/ 

uint16_t hash(uint16_t a) {
  uint32_t tem;
  tem =a;
  tem = (a+0x7ed55d16) + (tem<<12);
  tem = (a^0xc761c23c) ^ (tem>>19);
  tem = (a+0x165667b1) + (tem<<5);
  tem = (a+0xd3a2646c) ^ (tem<<9);
  tem = (a+0xfd7046c5) + (tem<<3);
  tem = (a^0xb55a4f09) ^ (tem>>16);
   return tem & 0xFFFF;
}

/*---------------------------------------------------------------------------*/
// ma hoa 64 bytes
void 
encrypt_cbc(uint8_t* data_input, uint8_t* data_output, const uint8_t* key, const uint8_t* iv) 
{ // ma hoa 64 bytes
    uint8_t data_temp[MAX_LEN_CBC];
    memcpy(data_temp, data_input, MAX_LEN_CBC);

    AES128_CBC_encrypt_buffer(data_temp, data_output, 64, key, iv);

}
/*---------------------------------------------------------------------------*/
// giai ma 64 bytes
void 
decrypt_cbc(uint8_t* data_input, uint8_t* data_output, const uint8_t* key, const uint8_t* iv) 
{
  uint8_t data_temp[MAX_LEN_CBC];
  memcpy(data_temp, data_input, MAX_LEN_CBC);

  AES128_CBC_decrypt_buffer(data_temp+0,  data_output+0,  16, key, iv);;
  AES128_CBC_decrypt_buffer(data_temp+16, data_output+16, 16, 0, 0);
  AES128_CBC_decrypt_buffer(data_temp+32, data_output+32, 16, 0, 0);
  AES128_CBC_decrypt_buffer(data_temp+48, data_output+48, 16, 0, 0);

}
/*---------------------------------------------------------------------------*/
#if DEBUG==1
void 
PRINTF_DATA(frame_struct_t *frame){ 
	int j;
	PRINTF("----------------------------------------------------------------\n");
	PRINTF("----------------------------------------------------------------\n");
	PRINTF("FLAG = 0x%02x\n", frame->sfd);
	PRINTF("len = %d\n", frame->len);
	PRINTF("seq = %ld\n", frame->seq);	
	PRINTF("state = 0x%02x\n", frame->state);
	PRINTF("cmd = 0x%02x\n", frame->cmd);

	PRINTF("\npayload_data [HEX] :\n");
	for (j =0 ; j < 16; j++){
		PRINTF(" %02x ", frame->payload_data[j]);
	}

	PRINTF("\nipv6_source_addr [HEX] :\n");
	for (j =0 ; j < 16; j++){
		PRINTF(" %02x ", frame->ipv6_source_addr[j]);
	}

	PRINTF("\nipv6_dest_addr [HEX] :\n");
	for (j =0 ; j < 16; j++){
		PRINTF(" %02x ", frame->ipv6_dest_addr[j]);
	}

	PRINTF("\nfor_future [HEX] :\n");
	for (j =0 ; j < 6; j++){
		PRINTF(" %02x ", frame->for_future[j]);
	}

	PRINTF("\ncrc16 = 0x%04x\n", frame->crc);

	PRINTF("----------------------------------------------------------------\n");
	PRINTF("----------------------------------------------------------------\n");
}
#else /* DEBUG */
void 
PRINTF_DATA(frame_struct_t *frame){
	;
}
#endif /* DEBUG */

/*-----------------------------------------------------------------------------------*/