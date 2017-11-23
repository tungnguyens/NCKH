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

void 
phex_8(uint8_t* data_8) 
{ // in chuoi hex 16 bytes
    unsigned char i;
    printf("\n");
    for(i = 0; i < 8; i++)
        printf("%.2x  ", data_8[i]);
    printf("\n");
}
/*---------------------------------------------------------------------------*/
void 
phex_64(uint8_t* data_64) 
{ // in chuoi hex 64 bytes
    unsigned char i;
    printf("phex_64\n");
    for(i = 0; i < 8; i++) 
        phex_8(data_64 + (i*8));
    printf("\n");
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

void copy_node2node(node_t *node_scr, node_t *node_des){
    node_des->stt = node_scr->stt;
    //PRINTF("STT: %d\r\n", node_des->stt);
    node_des->type_device = node_scr->type_device;
    //PRINTF("Type: %d\r\n", node_des->type_device);
    memcpy(&node_des->ipv6_addr, &node_scr->ipv6_addr, 26);
    //PRINTF("IPv6 Addr: %s\r\n", node_des->ipv6_addr);
    node_des->connected = node_scr->connected;
    //PRINTF("connected??? : %c\r\n", node_des->connected);
    node_des->emergency = node_scr->emergency;

    node_des->num_receive = node_scr->num_receive;
    //PRINTF("Receive: %d\r\n", node_des->num_receive);
    node_des->num_send = node_scr->num_send;
    //PRINTF("Send: %d\r\n", node_des->num_send);
    node_des->num_emergency = node_scr->num_emergency;
    //PRINTF("Emergercy: %d\r\n", node_des->num_emergency);
    node_des->last_seq = node_scr->last_seq;

    memcpy(&node_des->RSSI, &node_scr->RSSI, MAX_DEVICE +1);

    memcpy(&node_des->last_data_receive, &node_scr->last_data_receive, sizeof(frame_struct_t));

    memcpy(&node_des->last_data_send, &node_scr->last_data_send, sizeof(frame_struct_t));
    node_des->challenge_code = node_scr->challenge_code;
    //PRINTF("STT: %d", node_des->stt);
    node_des->last_pos = node_scr->last_pos;
    //PRINTF("STT: %d", node_des->stt);
    node_des->authenticated = node_scr->authenticated;
    //PRINTF("STT: %d", node_des->stt);
    memcpy(&node_des->key, &node_scr->key, 16);
    //PRINTF("STT: %d", node_des->stt);
    node_des->timer = node_scr->timer;
}

void PRINTF_DATA_NODE(node_t *node_id){
    uint8_t i;
    PRINTF(">>>>>>>>>>>-------------------------------------------<<<<<<<<<<<\n");
    PRINTF(">>>>>>>>>>>-------------------------------------------<<<<<<<<<<<\n");
    PRINTF("Thu tu Device: %d\r\n", node_id->stt);
    if(node_id->type_device == 'R')
        PRINTF("Device la: Router\r\n");
    else if(node_id->type_device == 'E')
        PRINTF("Device la: End Device\r\n");
    PRINTF("IPv6 Address la: %s\r\n",node_id->ipv6_addr);
    PRINTF("Trang thai Device: %c\r\n",node_id->connected);
    PRINTF("so luong data da nhan: %d\r\n", node_id->num_receive);
    PRINTF("so luong data da gui: %d\r\n", node_id->num_send);
    PRINTF("so luong data khan cap: %d\r\n", node_id->num_emergency);
    PRINTF("Last Data receive: \r\n");
    PRINTF_DATA(&node_id->last_data_receive);
    PRINTF("\r\n");
    PRINTF("Last Data send: \r\n");
    PRINTF_DATA(&node_id->last_data_send);
    PRINTF("\r\n"); 
    PRINTF("Challenge code: %04x",node_id->challenge_code);
    PRINTF("\r\n");
    PRINTF("Vi tri: %02x\r\n", node_id->last_pos);
    PRINTF("Xac thuc Device: %c\r\n",node_id->authenticated);
    PRINTF("Key cung cap cho Device: ");
    for (i = 0; i < 16; i++)
        PRINTF("%02x ",node_id->key[i]);
    PRINTF("\r\n");
    PRINTF(">>>>>>>>>>>-------------------------------------------<<<<<<<<<<<\n");
    PRINTF(">>>>>>>>>>>-------------------------------------------<<<<<<<<<<<\n");

}

//-------------------------------------------------------------
void ipv6_to_str_unexpanded(char * str, const struct in6_addr * addr) {
    sprintf(str, "%02x%02x::%02x%02x:%02x%02x:%02x%02x:%02x%02x",
        (int)addr->s6_addr[0], (int)addr->s6_addr[1],
        (int)addr->s6_addr[8], (int)addr->s6_addr[9],
        (int)addr->s6_addr[10], (int)addr->s6_addr[11],
        (int)addr->s6_addr[12], (int)addr->s6_addr[13],
        (int)addr->s6_addr[14], (int)addr->s6_addr[15]);
    PRINTF("IPV6_TO_STR = %s\r\n", str);
}
void
prepare2send(frame_struct_t *send, uint8_t *src_ipaddr, uint8_t *dest_ipaddr,\
            uint32_t frame_counter, uint8_t state, uint8_t cmd, uint8_t *data){

    send->sfd = SFD;
    send->len = MAX_LEN;
    send->seq = frame_counter;
    send->state = state;
    send->cmd = cmd;

    memcpy(send->payload_data, data, 16);
    memcpy(send->ipv6_source_addr, src_ipaddr, 16);
    memcpy(send->ipv6_dest_addr, dest_ipaddr, 16);
    memset(send->for_future, 0, 6);

    send->crc = gen_crc16((uint8_t *)send, MAX_LEN - 2);

    //PRINTF_DATA(send);

}

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
uint16_t gen_random_num() {
    uint16_t random1, random2;
    random1 = rand() % 254 +1;
    random2 = rand() % 254 +1;    
    return (random1<<8) | (random2);   
}
/*---------------------------------------------------------------------------------*/
void gen_random_key_128(unsigned char* key){
    int i;
    unsigned char byte_array[16];
    for (i=0; i<16; i++) {
        byte_array[i] = gen_random_num() & 0xFF;
    }
    strcpy(key,byte_array); 
}
/*--------------------------------------------------------------------------------*/ 

void my_memcpy(uint8_t *dest, uint8_t *src, int len){
    int i;
    for(i=0; i<len; i++){
        *(dest+i) = *(src+i);
    }
}
/*--------------------------------------------------------------------------------*/ 
uint8_t check_router_list(uint8_t *router_ipaddr){
    int c, i, j;
    static uint8_t my_router_ipaddr[LEN_CHAR_IPV6];
    static const char router[] = "router.txt";
    FILE *router_list; 

    memset(my_router_ipaddr, 0, LEN_CHAR_IPV6);
    //PRINTF("router = %s\n", router_ipaddr);

    router_list = fopen ( router, "r" );

    i=0; j=1;

    if ( router_list != NULL ) {
        while((c = fgetc(router_list)) != EOF) {
            if (c != 0x0a){
                my_router_ipaddr[i] = c;
                //PRINTF("%c",my_router_ipaddr[i]);
                i++;
            }
            else {  
                i = 0;
                //PRINTF("\nmy router = %s\n", my_router_ipaddr);           
                if(strcmp ( router_ipaddr, my_router_ipaddr) == 0){
                    //PRINTF("vi tri router = %d\n", j);
                    fclose ( router_list );
                    return j;
                }
                j++;
            }
        }        
        fclose ( router_list );            
    }
    else {
        perror ( router ); /* why didn't the file open? */
        fclose ( router_list ); 
    }
    //PRINTF("Ko thuoc Router list\n");
    return 0;
}
/*--------------------------------------------------------------------------------*/ 
uint8_t check_end_device_list(uint8_t *end_device_ipaddr){
    int c, i, j;
    static uint8_t my_end_device_ipaddr[LEN_CHAR_IPV6];
    static const char end_device[] = "end-device.txt";
    FILE *end_device_list; 

    memset(my_end_device_ipaddr, 0, LEN_CHAR_IPV6);
    //PRINTF("end-device = %s\n", end_device_ipaddr);

    end_device_list = fopen ( end_device, "r" );

    i=0; j=1;

    if ( end_device_list != NULL ) {
        while((c = fgetc(end_device_list)) != EOF) {
            if (c != 0x0a){
                my_end_device_ipaddr[i] = c;
                //PRINTF("%c",my_end_device_ipaddr[i]);
                i++;
            }
            else {  
                i = 0;
                //PRINTF("\nmy End device = %s\n", my_end_device_ipaddr);           
                if(strcmp ( end_device_ipaddr, my_end_device_ipaddr) == 0){
                    //PRINTF("vi tri End device = %d\n", j);
                    fclose ( end_device_list );
                    return j;
                }
                j++;
            }
        }        
        fclose ( end_device_list );            
    }
    else {
        perror ( end_device ); /* why didn't the file open? */
        fclose ( end_device_list ); 
    }
    //PRINTF("Ko thuoc End device list\n");
    return 0;
}

/*--------------------------------------------------------------------------------*/ 

bool checklist(uint8_t *device_ipaddr) //JHoang anh
{ 
    router_pos = check_router_list(device_ipaddr);
    end_device_pos = check_end_device_list(device_ipaddr);                
    if(router_pos){
        PRINTF("This router is the %d\n", router_pos);
        copy_node2node(&router[router_pos], &node_alt);
        return ACCEPT;
    }      
    else if(end_device_pos){
        PRINTF("This end device is the %d\n", end_device_pos);
        //memcpy(&end_device[end_device_pos], &node_alt, sizeof(node_t));
        copy_node2node(&end_device[end_device_pos], &node_alt);
        //PRINTF(">>>>>>>>>>HERE<<<<<<<<<<\r\n");
        //PRINTF_DATA_NODE(&end_device[end_device_pos]);
        //PRINTF(">>>>>>>>>>END_HERE<<<<<<<<<<\r\n");
        return ACCEPT;
    } 
    else{
        PRINTF("This is not my device\n");
        return DENY;
    }
}
/*--------------------------------------------------------------------------------*/ 
bool check_statedata_rev (char *rev_buffer) //hoang anh
{
    uint8_t i,count=0;
    for(i=56;i<62;i++){
        if(rev_buffer[i]==0xffffffff){
            count++;
        }
    }
    if(count==6){
        return STATE_BEGIN;
    }
    else{
        return 0;
    }
}
/*--------------------------------------------------------------------------------*/

bool check_seq(uint32_t new_seq,uint32_t old_seq) //hoang anh
{
    if(new_seq<=old_seq)
        return DENY;
    else
        return ACCEPT;
}
/*--------------------------------------------------------------------------------*/

void PRINT_ALL()
{
     uint8_t i, j;
    double total_rev = 0;
    double total_last_seq = 0;

    // printf("|----|---------------------------|---|---|-----|-----|-----|-----|-------|----------------------------------|---------------------------------|\n");
    // printf("|node|       ipv6 address        |TYP|AUT| num | num | num | last| chall |             key[16]              |          Last time seen         |\n");
    // printf("| id |  (prefix: aaaa::0/64)     |   |   |send | recv| emer| seq | -code |              (hex)               |              (GMT+7)            |\n");
    // printf("|----|---------------------------|---|---|-----|-----|-----|-----|-------|----------------------------------|---------------------------------|\n");

    printf("|----|---------------------------|---|---|-----|-----|-----|-----|-----|---------------------------------|\n");
    printf("|node|       ipv6 address        |TYP|AUT| num | num | num | last| BPM |          Last time seen         |\n");
    printf("| id |  (prefix: aaaa::0/64)     |   |   |send | recv| emer| seq |     |             (GMT+7)             |\n");
    printf("|----|---------------------------|---|---|-----|-----|-----|-----|-----|---------------------------------|\n");

    for(i = 1; i < MAX_DEVICE + 1; i++){
    printf("| %2d | %25s | %c | %c |%5d|%5d|%5d|%5d| %2d",\
            my_device[i].stt, \
            my_device[i].ipv6_addr, \
            my_device[i].type_device, \
            my_device[i].authenticated, \
            my_device[i].num_send, \
            my_device[i].num_receive, \
            my_device[i].num_emergency, \
            my_device[i].last_seq,\
            my_device[i].last_data_receive.payload_data[0]);
            //my_device[i].challenge_code);
    // for(j=0; j<16; j++){
    //     printf("%02x",my_device[i].key[j]);
    // }

    // for(i = 1; i < MAX_DEVICE + 1; i++){
    //     printf("| %2d | %25s | %c/%c |%c|%5d|%5d|%5d|%5d| 0x%04x| ",\
    //             my_device[i].stt, \
    //             my_device[i].ipv6_addr, \
    //             my_device[i].connected, \
    //             my_device[i].authenticated, \
    //             my_device[i].type_device,\
    //             my_device[i].num_send, \
    //             my_device[i].num_receive, \
    //             my_device[i].num_emergency, \
    //             my_device[i].last_seq,\
    //             my_device[i].challenge_code);
    //     for(j=0; j<16; j++){
    //         printf("%02x",my_device[i].key[j]);
    //     }
        printf(" |     %.*s    |\n", 24, asctime (localtime ( &my_device[i].timer)));

    printf("|----|---------------------------|---|---|-----|-----|-----|-----|-----|---------------------------------|\n");
    }

    for(i = 1; i < MAX_DEVICE + 1; i++){
        total_rev = total_rev + (double) my_device[i].num_receive;
        total_last_seq = total_last_seq + (double) my_device[i].last_seq;
    }

    printf("\n--- >>>>>>> PRR = %.0lf / %.0lf = %lf \n", total_rev, total_last_seq, total_rev/total_last_seq);
}

/*--------------------------------------------------------------------------------*/
/***********************************************************************************/
void sort_increase(double a[],uint32_t length){
  uint32_t i,j;
  double temp=0;
  //printf("sort:****\n");
  for(i=0;i<length-1;i++){
    for(j=i+1;j<length;j++){
      if(a[i]>a[j]){
        temp=a[i];
        a[i]=a[j];
        a[j]=temp;
      }
    }
  }
}
/***********************************************************************************/
void update_data(room *des,double source[][ROW_MAX][COLUM_MAX], uint8_t length){
 uint32_t i,j,z;
 for(z=0;z<length;z++){
    for(i=0;i<ROW_MAX;i++){
        for(j=0;j<COLUM_MAX;j++){
        des->room[z][i][j]=source[z][i][j];
        }
    }
 }
}
/***********************************************************************************/
uint8_t min_max_value(double data[],uint32_t length,uint8_t type)
{   uint32_t i;
    double temp[length];

    for(i=0;i<length;i++){
        temp[i]=data[i];\
    }

    sort_increase(data,length);

    for(i=0;i<length;i++){
        if(type==MAX){
            if(temp[i]==data[length-1]){
              //printf("dang o room %d\n",i );
            return i;
            }
        }
        if(type==MIN){
            if(temp[i]==data[0]){
            return i;
            }
        }
    }
}
/***********************************************************************************/
uint8_t locate_KNN(double datainput[],room *data_cp, uint8_t k ){
  uint32_t i,j,z;
  double r[MAX_POSITION][ROW_MAX]={};
  uint8_t count=0;
  double save[MAX_POSITION*ROW_MAX];
  double rm[MAX_POSITION]={};
  uint8_t loc;
  printf("LOCATE_KNN\n");

    for(z=0;z<MAX_POSITION;z++){
        for(i=0;i<ROW_MAX;i++){
            for(j=0;j<COLUM_MAX;j++){
                r[z][i]=r[z][i]+(datainput[j]-data_cp->room[z][i][j])*(datainput[j]-data_cp->room[z][i][j]);
            }
        }
    }

  printf("PROCESS DATA \n");
    for(j=0;j<MAX_POSITION;j++){
        for(i=0;i<ROW_MAX;i++){
            save[i+j*ROW_MAX]=r[j][i];
        }
    }

    printf("TIM MAX\n");  
    sort_increase(save,MAX_POSITION*ROW_MAX);

  for(z=0;z<NUMBER_K;z++){
        for(i=0;i<MAX_POSITION;i++){
            for(j=0;j<ROW_MAX;j++){                   
                if(save[z]==r[i][j]){
                    rm[i]=rm[i]+1;                   
                }                
            }
        }
  }

  for(i=0;i<MAX_POSITION;i++){
        printf("rm[%d]=%.2lf\n",i,rm[i]);
  }

  return min_max_value(rm,MAX_POSITION,MAX);
}
/***********************************************************************************/
uint8_t locate_euclid(double datainput[],room *data_cp)
{
    uint32_t i,j,z;
    double mean[MAX_POSITION][COLUM_MAX]={};
    double distance[MAX_POSITION]={};
    for(z=0;z<MAX_POSITION;z++){
      for(i=0;i<ROW_MAX;i++){
      for(j=0;j<COLUM_MAX;j++){
              mean[z][j]=mean[z][j]+data_cp->room[z][i][j];
      }
      }
    }

    for(i=0;i<MAX_POSITION;i++){
        for(j=0;j<COLUM_MAX;j++){
            mean[i][j]=mean[i][j]/ROW_MAX;
        }
    }

    for(i=0;i<MAX_POSITION;i++){
        for(j=0;j<COLUM_MAX;j++){
            distance[i]=distance[i]+(mean[i][j]-datainput[j])*(mean[i][j]-datainput[j]);
        }
    }

    return min_max_value(distance,MAX_POSITION,MIN);
}
/************************************************************************************************/
uint8_t bayes_locate( double datainput[],room *data_cp )
{
    //0-85 0,(0-10) , [10-20),[20-30),[30,40),[40,50),[50-60),[60,70),[70,80]
    double P_room[MAX_POSITION];
    uint32_t i,j,z,pos,t;
    double comp[MAX_POSITION];
    double dem[MAX_POSITION]={};
    double PA[MAX_FEATURE][COLUM_MAX]={};
    double pro[MAX_POSITION][COLUM_MAX]={};
    double P_sig_room[MAX_POSITION];
    signal signal[MAX_POSITION];
    double mean[MAX_POSITION][COLUM_MAX]={};
    double var[MAX_POSITION][COLUM_MAX]={};
    double dis[MAX_POSITION]={};
     for(z=0;z<MAX_POSITION;z++){
      P_room[z]=1;
     }
   // memset(&signal,0,sizeof(signal));
     for(z=0;z<MAX_POSITION;z++){
        for(i=0;i<MAX_FEATURE;i++){
            for(j=0;j<COLUM_MAX;j++){
            signal[z].sig[i][j]=0;
             // printf("test [%d][%d][%d] %lf  \n",z,i,j,signal[z].sig[i][j]);
            }
        }
      }
    for(z=0;z<MAX_POSITION;z++){
            for(j=0;j<COLUM_MAX;j++){
              for(i=0;i<ROW_MAX;i++){
                mean[z][j]=(data_cp->room[z][i][j])/(ROW_MAX)+mean[z][j]; //**********tinh mean
                //for(t=0;t<MAX_FEATURE;t++){
                    //if(data_cp->room[z][i][j]==t){
                     // signal[z].sig[t][j]++;
                  //  }
                }
              //  printf("mean cua router %d phong %d: %lf\n",j,z,mean[z][j] );
              }
             
            }
              //*************** tinh var
      for(z=0;z<MAX_POSITION;z++){
        
            for(j=0;j<COLUM_MAX;j++){
              for(i=0;i<ROW_MAX;i++){
 var[z][j]=var[z][j]+(data_cp->room[z][i][j]-mean[z][j])*(data_cp->room[z][i][j]-mean[z][j])/(ROW_MAX-1);             
         
          }
//printf("var cua router %d phong %d: %lf\n",j,z,var[z][j] );
        }

      }
    for(z=0;z<MAX_POSITION;z++){
            for(j=0;j<COLUM_MAX;j++){
                    for(i=0;i<MAX_FEATURE;i++){

                      if(var[z][j]==0.0)
                      {
                        dis[z]++; 
 //  printf("xac suat  %d->%d cua router %d phong %d:%lf\n",i-1,i,j,z,signal[z].sig[i][j] );
// dem so router mat ket noi cua phong z

                      }
                      else{
                            
                            signal[z].sig[i][j]=0.5*(1+erf(((double)i+1-mean[z][j])/sqrt(var[z][j]*2)))- 0.5*(1+erf(((double)i-mean[z][j])/sqrt(var[z][j]*2)));
                      }
                    }
                     // printf("xac suat  %d->%d cua router %d phong %d:%lf\n",i-1,i,j,z,signal[z].sig[i][j] );

                       // signal[z].sig[i][j]=signal[z].sig[i][j]/ROW_MAX; // i la dac tinh, j la router thu j

                       // printf("[%d][%d][%d]:%lf\n",z,i,j,signal[z].sig[i][j] );
                    }
                  }
                
              
                      //  printf("xac xuat dis router %d tu phong %d:%lf\n",j,z, signal[z].sig[0][j]);
                
                for(z=0;z<MAX_POSITION;z++){
                    for(j=0;j<COLUM_MAX;j++){
                    if(var[z][j]==0.0)
                      {
                        signal[z].sig[0][j]=1/dis[z]; 
                      }
                    }
                  }
            
        
            for(i=0;i<COLUM_MAX;i++){
                    for(j=0;j<MAX_FEATURE;j++){
                        for(z=0;z<MAX_POSITION;z++){
                       PA[j][i]=signal[z].sig[j][i]/MAX_POSITION+PA[j][i]; // tinh xac xuat P(A)
                    }
              // printf("Xac xuat Dac tinh %d cua router %d:  %lf\n",j,i,PA[j][i] );

            }

        }
        for(z=0;z<MAX_POSITION;z++){
                for(j=0;j<COLUM_MAX;j++){
                  for(t=0;t<MAX_FEATURE;t++){
                    if(t-1<=datainput[j]&&datainput[j]<t){
                                if(PA[t][j]!=0.0){
                                pro[z][j]=signal[z].sig[t][j]/(PA[t][j]*MAX_POSITION);
                               //printf(" xac xuat thuoc router thu [%d] phong [%d]= %lf\n", j,z,pro[z][j]);
                                }
                                else{
                 //  printf(" erro tai [%d] [%d]= %lf\n", z,j,signal[z].sig[t][j]);

                  }


                     } 

                  }
                //   printf("Xac xuat router [%d] cua phong [%d]: %lf\n",j,z,pro[z][j] );

                            
              }
         //  printf("Xac xuat thuoc router [%d] cua phong [%d]: %lf\n",j,z,pro[z][j] );
        }
        
        
            
        for(z=0;z<MAX_POSITION;z++){
                for(j=0;j<COLUM_MAX;j++){
                 // printf("Xac xuat P_room [%d][%d]: %lf\n",z,j,pro[z][j]);
                if(pro[z][j]!=0.0){
                    P_room[z]=pro[z][j]*P_room[z];
                    
                  }
                  // else{

                //  printf("Xac xuat erro [%d][%d]: %lf\n",z,j,pro[z][j]);
                //}
                }
               
      //if(P_room[z]==1.0)
     // {
      // P_room[z]=0;
      // printf("Xac xuat phong %d la:%lf\n", z,P_room[z]);
      //}
                   
           // printf("Xac xuat phong %d la:%lf\n", z,P_room[z]);
       }
                    
        return min_max_value(P_room,MAX_POSITION,MAX);

}
/***********************************************************************************/
/*---End    code---*/
/*------------------------------------------------------*/

#if DEBUG==1
void 
PRINTF_DATA(frame_struct_t *frame){ 
	int j;
	PRINTF("----------------------------------------------------------------\n");
	PRINTF("----------------------------------------------------------------\n");
	PRINTF("FLAG = 0x%.2x\n", frame->sfd);
	PRINTF("len = %d\n", frame->len);
	PRINTF("seq = %d\n", frame->seq);
	PRINTF("state = 0x%.2x\n", frame->state);
	PRINTF("cmd = 0x%.2x\n", frame->cmd);

	PRINTF("\npayload_data [HEX] :\n");
	for (j =0 ; j < 16; j++){
		PRINTF(" %.2x ", frame->payload_data[j]);

	}

	PRINTF("\nipv6_source_addr [HEX] :\n");
	for (j =0 ; j < 16; j++){
		PRINTF(" %.2x ", frame->ipv6_source_addr[j]);
	}

	PRINTF("\nipv6_dest_addr [HEX] :\n");
	for (j =0 ; j < 16; j++){
		PRINTF(" %.2x ", frame->ipv6_dest_addr[j]);
	}

	PRINTF("\nfor_future [HEX] :\n");
	for (j =0 ; j < 6; j++){
		PRINTF(" %.2x ", frame->for_future[j]);
	}

	PRINTF("\ncrc16 = 0x%.4x\n", frame->crc);

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
void convert_hex2str(uint8_t *hex, char *str){
    printf("\n__convert_hex2str\n");
    sprintf(str, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
        hex[0],
        hex[1],
        hex[2],
        hex[3],
        hex[4],
        hex[5],
        hex[6],
        hex[7],
        hex[8],
        hex[9],
        hex[10],
        hex[11],
        hex[12],
        hex[13],
        hex[14],
        hex[15]);
    //str[sizeof(str)]= '\0';
    PRINTF("hex2str = %s\r\n", str);
}