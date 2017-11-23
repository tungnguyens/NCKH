//end-device

etimer_set(&et, 60*clock_seconds);
while(state == EMERGENCY){
	PROCESS_YIELD();
	if(etimer_expired(&et)){
		send_emergency();
		etimer_restart(&et);
	}
}

void send_emergency(){
	get_list_neighbour();
	send2router(); // key_begin = "tung_duc_anh_anh" giai ma va ma hoa cbc 64 bytes = key chung
}