/* 
 * A server that returns your public IP address upon an http request. 
 * The main purpose is to make scripting easier involving IP easier.
 *
 * */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9999


void _abort(char* errormsg) {

	puts(errormsg);
	exit(0);

}

int main() {

	//buffer to store incoming request to
	char buf[10000];

	//starting socker, socket to redirect to
	struct sockaddr_in servaddr, rediraddr;
	//new socket size
	socklen_t newsocket_size;
	int sockfd, bound, redirsock;

	//process id for fork
	pid_t forkid;

	//allocate socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//check if successful
	if (sockfd < 0) {
	
		_abort("Unable to allocate socket\n");	
	
	}

	memset(&servaddr, '\0', sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	bound = bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));


	//try to bind socket
	if(listen(sockfd, 10) == 0){
		printf("Bound socket!");
	}else{
		_abort("[-]Error in binding.\n");
	}


	while(1){

		//accept new connection
		redirsock = accept(sockfd, (struct sockaddr*)&rediraddr, &newsocket_size);
		if(redirsock < 0){
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(rediraddr.sin_addr), ntohs(rediraddr.sin_port));

		
		if((forkid = fork()) == 0){
			//terminate previous input socket
			close(sockfd);

			while(1){

				//copy incoming message to buffer
				recv(redirsock, buf, 10000, 0);

				//just print buffer for now
				puts(buf);

			}
		}

	}

	close(redirsock);


	return 0;




}

