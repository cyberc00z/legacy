#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define PORT 8080


int main(int argc, char const *argv[]){


	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char *o = "Oye from CLient";
	char buffer[1024] = {0};
	if ((sock  = socket(AF_INET, SOCK_STREAM, 0)) < 0){
	
		printf("\n Socket Creation error \n");
		return -1;

	}

	serv_addr.sin_family= AF_INET;
	serv_addr.sin_port = htons(PORT);
	
	// Convert IPv4 and IPv6 from text to binary
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0){
	
		printf("\nInvalid address/ Address not support\n");
		return -1;

	}
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)        {

	          printf("Connection failed/n")	;
		  return -1;
		
	}
	send(sock, o, strlen(o), 0);
	printf("Oye dm sent\n");
	valread = read(sock, buffer, 1024);
	printf("%s\n", buffer);
	return 0;
}

