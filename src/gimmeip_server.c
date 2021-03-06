/* 
 * A server that returns your public IP address upon an http request. 
 * The main purpose is to make scripting easier involving IP easier.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/sendfile.h>

#include "html_content.h"

#define PORT 8888 
#define HOST_ADDR "127.0.0.1" 
#define TIMEOUT_SEC 3

void _abort(char* errormsg) {

	puts(errormsg);
	exit(1);

}


struct connection_data{

	int redirsock; 
	char* buf;
       	char* html_to_serve_template; 
	int html_len; 
	int header_len; 
	int full_msg_len; 
	char* resp_200;
	struct sockaddr_in rediraddr;

};



void handle_curl(int redirsock, char* ipaddr) {

	//convert ip address to string and send it
	send(redirsock, ipaddr, strlen(ipaddr), 0);
	bzero(ipaddr, sizeof(ipaddr));
	close(redirsock);

}


void handle_favicon(int redirsock) {

	//lets send an 200 response first for compatibility
	int fdimg = open("favicon.ico", O_RDONLY);
	sendfile(redirsock, fdimg, NULL, 7000);
	close(fdimg);
	close(redirsock);

}


void handle_404(int redirsock) {

	//convert ip address to string and send it
	char resp_404[] = "HTTP/1.1 404\r\n^\
			  Content-Length: 1000\r\n^\
			  Connection: close\r\n^\
			  <link rel='icon' type='image/png' href='/favicon.ico' />\r\n^\
			  Content-Type: text/html\r\n\r\n^\
			  <!DOCTYPE html>\r\n^\
			  <html><head>\r\n^\
			  <title>404</title>\r\n^\
			  </head><body><center>\r\n^\
			  <h1>404</h1>\r\n^\
			  </center></body></html>\r\n";
	
	//tokenize data and send it line by line
	char* line_ptr = strtok(resp_404,"^");
	while(line_ptr) {

		send(redirsock, line_ptr, strlen(line_ptr), 0);
		line_ptr = strtok(NULL, "^");

	}
	close(redirsock);

}

void handle_html(char* html_to_serve_template, int html_len, int header_len, 
		char* resp_200, char* ipaddr, int redirsock, int full_msg_len) {

	//lets make a copy to modify in the original html string
	char* html_to_serve = (char*)malloc(full_msg_len+1);
	memcpy(html_to_serve, resp_200, header_len);
	memcpy(html_to_serve+header_len, html_to_serve_template, html_len + 1);

	//lets replace the dummy ip address in the html string
	char* dummy_ip_ptr = strstr(html_to_serve, "^ip^           ");
	
	//check to make sure ip address is within length limits
	int ip_len = strlen(ipaddr);
	if (ip_len > 20) _abort("Malformatted IP address!\n"); 
	memcpy(dummy_ip_ptr,ipaddr,ip_len);

	//now send the html page with the user's IP in there
	send(redirsock, html_to_serve, full_msg_len, 0);
	bzero(ipaddr, sizeof(ipaddr));
	free(html_to_serve);
	
	close(redirsock);

}

void* handle_connection(void* in_conndata) {

	char* buf = (char*)malloc(1000);
	struct connection_data* conndata = (struct connection_data*) in_conndata;
	//copy incoming message to buffer
	read(conndata->redirsock, buf, 1000);
	//create a new buffer to send the ip address in
	char* ipaddr;
	ipaddr = inet_ntoa(conndata->rediraddr.sin_addr);

	//get the user agent, check if its curl
	char* user_agent_start;
	user_agent_start = strstr(buf,"User-Agent:");
	
	//send no reply if the header doesn't contain a user agent line
	if(user_agent_start != NULL){

		if( strncmp(user_agent_start, "User-Agent: curl", 16) == 0 ) {
		
			handle_curl(conndata->redirsock, ipaddr);
		
		} else {
	
			//is it a request for favicon?
			if (!strncmp(buf, "GET /favicon.ico", 16)) {
		
				handle_favicon(conndata->redirsock);

			} else if (!strncmp(buf, "GET / ", 6)) {

				//is it a request for the main page then?
				handle_html(conndata->html_to_serve_template,
						conndata->html_len,
						conndata->header_len,
						conndata->resp_200,
						ipaddr,
						conndata->redirsock,
						conndata->full_msg_len);
			} else {
				
				//if none of these, throw a 404
				handle_404(conndata->redirsock);

			}
		}

	}

	free(buf);
	return NULL;
}


int main() {

	//the timeout for receive operations
	struct timeval timeout;      
	timeout.tv_sec = TIMEOUT_SEC;
	timeout.tv_usec = 0;

	//html response if user agent not curl
	char* html_to_serve_template = get_html_template(); 
	int html_len = strlen(html_to_serve_template);

	//OK response header
	char resp_200[200];
	char* resp_200_first = "HTTP/1.1 200 OK\nConnection: close\nContent-Length: ";
	char* resp_200_last = "\nContent-Type: text/html\n\n";

	//stores the int representing the size of the input html file
	char html_contlength[50];
	sprintf(html_contlength, "%i", html_len);
	strcat(resp_200, resp_200_first);
	strcat(resp_200, html_contlength);
	strcat(resp_200, resp_200_last);
	int header_len = strlen(resp_200);

	//the full message length
	int full_msg_len = html_len + header_len;

	//starting socker, socket to redirect to
	struct sockaddr_in servaddr, rediraddr;
	//new socket size
	socklen_t newsocket_size;
	int server_socket, bound, redirsock;

	//allocate socket
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	//check if successful
	if (server_socket < 0) {
	
		_abort("Unable to allocate socket\n");	
	
	}

	memset(&servaddr, '\0', sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(HOST_ADDR);

	bound = bind(server_socket, (struct sockaddr*)&servaddr, sizeof(servaddr));


	//try to bind socket
	if(listen(server_socket, 10) == 0){
		printf("Bound socket %i!\n", PORT);
	}else{
		_abort("Unable to bind socket.\n");
	}


	while(1){

		//accept new connection
		redirsock = accept(server_socket, (struct sockaddr*)&rediraddr, &newsocket_size);

		//set socket receive timeout
		if (setsockopt (redirsock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                	sizeof(timeout)) < 0)
	 	       puts("setsockopt failed\n");


		if(redirsock < 0){
			printf("Unable to accept connection\n");
			
		} else {

			//this gets redirected to a log file
			printf("Connection accepted from %s:%d\n", inet_ntoa(rediraddr.sin_addr), ntohs(rediraddr.sin_port));

			//fire up some new threads
			pthread_t t;
			struct connection_data* conndata = malloc(sizeof(struct connection_data));
			conndata->redirsock = redirsock;
			conndata->html_to_serve_template = html_to_serve_template;
			conndata->html_len = html_len;
			conndata->header_len = header_len;
			conndata->full_msg_len = full_msg_len;
			conndata->resp_200 = resp_200;
			conndata->rediraddr = rediraddr;

			pthread_create(&t, NULL, handle_connection, (void*)conndata);
			pthread_join(t,NULL);


		}
	}

	return 0;

}

