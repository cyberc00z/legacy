/*  sock.h module for server
 *  Date : 28-March-2021
 *  
 **/
bool send_to_network(char *buffer, int client_id){

	if (send(c[client_id].c_socket, buffer, STRLEN, 0)==SOCKET_ERROR){
	
		newMessage("Error Sending Data");
	}
	return true;
}

bool recv_from_network(char *buffer){

	int i = recv(Socket, buffer, STRLEN, 0);
	buffer[i] = '\0';
	return true;
}

bool s_recv_data(char *buffer, int client_id){

	int i= recv(c[client_id].c_socket, buffer, STRLEN, 0);
	buffer[i] = '\0';
	return true;
}

void disconnect_client(int x)
{

	if (c[x].connected==true)
	{
	
		char ClMsgs[30], str[5];
		newMessages("A client Logs out");
		c[x].active = false;

	}
	else
		printf("\nCouldn't disconnect client :");
}

// start server ... bind port  and listen to incoming connections

void start_server(){

	newMessage("Starting Server");
	sockaddr_in server_address;
	server_address.sin_family=AF_INET;
        server_address.sin_addr.s_addr=inet_addr("0.0.0.0");
	server_address.sin_port=htons(DEFAULT_PORT);

	if (bind(Socket, (sockaddr *)&server_address, sizeof(server_address)) ==SOCKET_ERROR){
	
		newMessage("Failed to bind the socket");
		system("clear");
		exit(11);
	}
	else {
	
		*(program_msg+no_of_msg++) = "Socket Binded"
			flag_newMsg = true;
	}

	if(listen(SOCKET, 1) == SOCKET_ERROR)
	{
	
		newMessage("error listening on socket");
		system("clear");
		exit(0);
	}

	else {
	
		newMessage("Ready to accept Connections");
	}

}


