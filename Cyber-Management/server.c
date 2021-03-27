/*   Server                                              Client
 *    ||                                                  [Socket]
 *   Socket                                                 |
 *    ||                                                    |
 *  setsockopt                                              |
 *    ||                                                    |
 *   Bind                                                   |
 *    ||                                                    |
 *   Listen------------------------------------------------Connect 
 *    ||                                                    |
 *   Accept                                                 |
 *    ||                                                    |
 *   Send/Recv--------------------------------------------Send/Recv
 * */


/*  1.  Socket Creation
 *      
 *      int sockfd = socket(domain, portocol, type);
 *      sockfd: socket descriptor ,an integer like file handler(fp)
 *      type : TCP[SOCK_STREAM] /UDP[SOCK_DGRAM]  
 *      ip
 *
 *  2. Setsockopt:
 *        int setsockopt(int sockfd, int level, int optname, const void *optval,socklen_t optlen);
 *
 * 3. Bind
 *     int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
 *     In the example code, we bind the server to the localhost, hence we use INADDR_ANY to specify the IP address.
 *  
 *  4. Listen
 *      int listen(int sockfd, int backlog);
 *      It puts the server socket in a passive mode, where it waits for the client to approach the server to make a connection. The backlog, defines the maximum length to which the queue of pending connections for sockfd may grow. If a connection request arrives when the queue is full, the client may receive an error with an indication of ECONNREFUSED.
 * 
 * 5. Accept:
 *    int new_socket = accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
 *    It extracts the first connection request on the queue of pending connections for the listening socket, sockfd, creates a new connected socket, and returns a new file descriptor referring to that socket. At this point, connection is established between client and server, and they are ready to transfer data.
 *
 *
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080


int main(int argc, char *argv[]){


	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};
	char *hell = "Welcome to the hell!";

	// Creating socket file description
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
	
		perror("socket failed!");
		exit(EXIT_FAILURE);
	}
	
	// setsockopt
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
	
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);


	// binding
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
	
		perror("bind_failed!");
		exit(EXIT_FAILURE);

	}
	if (listen(server_fd, 3) < 0){
	
		perror("listen");
		exit(EXIT_FAILURE);
	}
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0){
	
		perror("accept");
		exit(EXIT_FAILURE);
	}
	valread = read(new_socket, buffer, 1024);
	printf("%s\n", buffer);
	send(new_socket, hell, strlen(hell), 0);
	printf("Message Sent!\n");
	return 0;
}








