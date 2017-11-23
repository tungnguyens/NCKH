//router

while(state != STATE_BEGIN){
	/* ------ WAIT event  --------*/
	PROCESS_YIELD();
	/* ------ event TCP/IP -------*/
	if(ev == tcpip_event) { 
  		tcpip_emergency_handler();
	}
}

void tcpip_emergency_handler(){

	if(uip_newdata()) {
		kiemtralen; .. uip_len_data == 64
		giaima_cbc_64 key begin
		kiem_tra_type, cmd phai la emergency
		add RSSI vao data
		add router IP
		ma hoa cbc 64 byte theo key nhan dc begin
		gui border
	}

}