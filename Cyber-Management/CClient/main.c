#include "function.h";

int main(){

	system("clear");
	ip = fopen("ip_add.txt", "r");
	if (ip==NULL){
	
		ip=fopen("ip_add.txt", "w");
		printf("ENTER IP ADDRESS : ");
		printf("ENTER PORT NUMBER : ");
		printf("ENTER CLIENT NUMBER : ");
		fflush(stdin);
		fgets(ip1.ip_address,50,stdin);
		scanf("%d", &ip1.ip_address);
		scanf("%d", &ip1.port_no);
		scanf("%d", &ip1.client_no);
		fseek(ip,0,SEEK_END);
		fwrite(&ip1,sizeof(ip1),1,ip);

	}
	fclose(ip);
	assign_ip_address();

	if (!SOCKET_START){
	
		start_socket();
		connect_server();
		SOCKET_START=true;
	}
	start_log;
	log_stat=0;
	login_screen();

	start_menu():
		menu();
	        while(1){
		
			if(get_code() == 60){
			
				show_menu();
				break;
			}
		}
		char sel;
		while(1){
		
			switch(sel=tolower(getchar())){
			
				case '1':
					if(flag_menu==0){
					
						service_menu();
					}
					break;
				case '2':
					if(flag_menu==0){
					
						credits();
					}
					break;
				case '3':
					if (flag_menu==0){
					
						log_stat = 2;
						log_out();
					}
					break;
				case '27':
					if (flag_menu==1 || flag_menu==3||flag_menu==3||flag_menu==9){
					
						to_tray=4;

					}else
					if(flag_menu==0){
					
						to_tray=1;
					}
					break;
				case 'h':
					help_window():
					break;
				case 't':
					to_tray=1;
					break;

				default:
					break;
			}
			if(to_tray ==3){
			
				help_window();
			}
			else 
		        if(to_tray==1){
			
				break;
			}else
			if (to_tray==4){
			
				show_menu();
			}
			if (log_stat==2){break;}

		}
		if(log_stat==2){ goto start_log;  }
		if (to_tray==1) {goto start_menu;}

		return 0;

}
