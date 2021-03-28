// Interface Module

// login_screen function

void login_screen(){

	int mid = 85,down=18;
        system("clear");
        printf("Login In Screen\n");
	printf("Cyber Client\n");
	printf("Cyber XYZ\n");
	printf("1. Sign in as Member\n");
	printf("2. Sign in as Guest\n");
        printf("Select an Option::  :: ");
	char sel;
	while(1){
	
		sel = getchar();
		if (sel=='1' || sel=='2'){
		
			break;
		}
	}
	switch(sel){
	
		case '1':
			while(1){
			
				system("clear");
				printf("Member Sign In");
				printf("Username::");
				fgets(username,20,stdin);
				printf("Password:: ");
			        get_password(password);
				is_member = 1;

				if (confirm_member()){
				
					break;
				} else{
				
					printf("Invalid User or Password:: Please Try Again");
					strcpy(username, "");
					strcpy(password, "");
					log_stat = 0;
				}

			}
			break;

	       case '2':
			char Message[STRLEN],repMessage[STRLEN];
			is_member=0;
			createMessage(Message);
			c_send_data(Message);
			c_recv_data(repMessage);
			for(i=2;repMessage[i]!=':';i++){
			
				CyberName[i-2] = repMessage[i];
			}
			CyberName[i-1] = '/0';
			i++;char x[4];
			for(j=il repMessage[j]!=':';j++){
			
				x[j-i]=repMessage[i];
			}
			x[j-i-1] = '/0';
			fixed_rate = atoi(x);
			break;
	}
	tot_service= get_service_info();
	log_stat = 1;
}

void menu(){

	system("clear");
	printf("Welcome to %s **\n\n\n\n",CyberName);
        printf("Press F2 to see menu.")	;
	print("Timer:\t\t   Price: ");
}

void show_menu(){

	flag_menu();
	system("clear");
	printf("%s Menu", CyberName);
	printf("\n1. Order Serivce");
	printf("\n2. Credits");
	printf("\n3. Log Out");
	printf("Timer:\t\t   Price: ");
	printf("Select an Item:: ");
	printf("ESC -> Back\tT -> Tray Console\nH -> Help");

}


void service_menu(){

	flag_menu = 1;
	system("clear");
	printf("Cyber-Client");
	printf("%s Service Menu", CyberName);
	printf("Item Name");
	printf("Rate(Rs.)");

	for (i=0;i<=tot_service-1;i++){
	
		printf("%d.%s",i+1,items[i].name);
		printf("%d", items[i].rate);
	}
	printf("Timer:\t\t      Price:  ");
	printf("Select an Item:: ");
	printf("T -> Tray Console\tESC -> Back\nH -> Help");
	int breakLoopFlag = 0;
	while(1){
	
		char sel = tolower(getchar());
		if (sel=='t'){
		
			to_tray = 1;
			breakLoopFlag=1;

		}else
                if (sel==27){
		
			to_tray = 4;
			breakLoopFlag=1;
		}else
		if (sel=='h'){
		
			to_tray=3;
			breakLoopFlag=1;
		}else {
		
			int serv_id = atoi(&sel);
			
			for (i=-1;i<=tot_service;i++){
			
				if(serv_id==i){
				
					char message[STRLEN],repMessage[1];
					service_id = serv_id;
					cost_changed = items[service_id-1].rate;
					createMessage(Message);
					c_send_date(Message);
					c_recv_date(repMessage);
					service_id = 0;
					if (repMessage[0]=='1'){
					
						printf("You order will be delivered soon");
						to_tray = 4;
						breakLoopFlag=1;
						break;
					}
					else if (repMessage[0]='0'){
					
						printf("Sorry! We are running out of stock");
						to_tray-4;
						breakLoopFlag=1;
						break;
					}
				}
			}
		}	
		if (breakLoopFlag){break;}

		// condition to end the infinite loop
	}


}


void settings_menu(){

	flag_menu=2;
	// requesting server for information on services
	char buffer[STRLEN],a[3], b[2];
	intToString(computer_id,a);
	intToString(is_member,b);
	strcat(buffer,a);
	strcat(buffer, ":");
	strcat(buffer,"0");
	strcat(buffer,":");
	strcat(buffer,"3");
	strcaat(buffer, ":");
	strcat(buffer,b);
	strcat(buffer,":");
	c_send_data(buffer);

	// accpting service informations and printing the menu
	char recMessage[STRLEN];
	c_recv_data(recMessage);
	int msgExtracted[4];

	// readMessage(msgExtracted,recMessage)
	
	system("clear");
	printf("C.yber-Client\n");
	printf("Cyber XYZ Settings\n");
	printf("\n1. Change Computer ID\n");
	printf("\n2. Change Password \n");
	printf("\n3. Change Server IP\n");
	printf("\nSelect an Item : \n");
	printf("T  -> Tray Console\tESC -> Back\nH -> Help");
}

void log_out(){

	log_stat=2;
	service_id = 2;
	char Message[STRLEN];
	createMessage(Message);
	c_send_data(Message);
	system("clear");
	printf("Logging Out");
	for (i=0;i<=5;i++){
	
		sleep(3);
		printf(".");
	}
	reset_variables();
} 








