// socket related functions


// sending date function
bool c_send_data(char *buffer){

	if (send(Socket, buffer, STRLEN, 0)==SOCKET_ERROR)
	{
	
		printf("couldnot send");
	}
	return true;
}

bool c_recv_data(char *buffer){

	int i=recv(Socket, buffer, STRLEN, 0);
	buffer[i] = '\0';
	return true;
}



void connect_server(){

	sockaddr_in cl_address;
	fflush(stdin);
	int port;
	port = port_add;
	cl_address.sin_family=AF_INET;
	cl_address.sin_port = htons(port);
	cl_address.sin_addr.s_addr=inet_addr(ipaddress);
	if(connect(Socket,(sockaddr *)&cl_address, sizeof(cl_address)) == SOCKET_ERROR){
	
		printf("\n\a Error Connecting to Server\a\n");
		system("clear");
		client();

	}
	else {
	
		printf("\nConnected to server\n");
	}
}


void start_socket(){

	Socket = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);
	if (Socket==INVALID_SOCKET)
	{
	
		printf("Socket initialization : Error Creating while Socket\n");
		system("clear");
		exit(11);
	}
	else {
	
		printf("socket initializes successfully!");
	}
}








