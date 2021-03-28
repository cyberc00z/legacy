/*  interface.h for Server Side Interface
 *  Date : 28-march-2021
 *
 */

void menu(){

	FILE *sInfo;
	sInfo = fopen("./save/serverinfo.txt", "r");
	flag_menu=0;
	char sDate[13];
	int cn=7, i, count=9;
	system("clear");
	for (i=0;i<56;i++){
	
		gotoPrint(90,i,"|");
	}
	if (flag_reset==true){
	
		for (i=0;i<no_of_client;i++){
		
			c[i].r.num_request = 0;
		}
	}
	flag_reset = false;

	getDate(sDate);
	fscanf( sInfo,"%s", CyberName);
	gotoPrint(35, 1, cyberName);
	printf("Today : %s", sDate);
	gotoPrint(23, 3, "--------------");
	gotoPrint(25, 4, "1. Requests     2. Settings       3. Exit");

	gotoPrint(23, 5, "-----------------");
	gotoPrint(5, cn, "Clients");
	gotoPrint(20, cn,"Requests Pending");
	gotoPrint(40,cn,"Login Time");
	gotoPrint(55,cn,"Logout Time");
	gotoPrint(75,cn,"Cost");
	if (no_of_clients>0){
	
		for(i=0;i<no_of_client;i++){
		
			printf("%d", c[i].id);
			printf("%d", c[i].r.num_request);
			printf("%d:%d", c[i].logIn.hr,c[i].logIn.min);
			if(c[i].active == false){
			
				printf("%d:%d", c[i].logOut.hr,c[i].logOut.min);
				printf("Rs. %d", c[i].cost);
			}
		}
	}
	flag_newMsg = true;

}















