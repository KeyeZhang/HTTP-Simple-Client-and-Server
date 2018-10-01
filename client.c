/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 


#define HEAD "GET /%s HTTP/1.1\n\
User-Agent: Wget/1.12(linux-gnu)\n\
Host: %s:%s\n\
Connection: Keep-Alive\n\n"


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void get_input(char * input, char * filename, char * ipaddr, char * port){
	char * p_start;
	char * p_end;
	p_start = input;
	while(*p_start != '/'){
		p_start++;
	}
	p_start += 2;
	p_end = p_start;
	int flag = 0;
	int port_flag = 0;
	while(*p_end != '\x00'){
		if(*p_end == ':'){
			port_flag = 1;
			
			strncpy(ipaddr, p_start, p_end - p_start);
			p_end ++;
			p_start = p_end;
			continue;
		}
		if(flag == 0  && *p_end == '/'){
			
			if(port_flag == 1){
				strncpy(port, p_start, p_end - p_start);
			}
			else{
				strncpy(ipaddr, p_start, p_end - p_start);
			}
			p_end ++;
			p_start = p_end;
			flag = 1;
			
		}
		p_end++;
	}
	strncpy(filename, p_start, p_end - p_start);
//	return;
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	
	char filename[128];
	char port[8];
	//char * path = NULL;
	char ipaddr[16];
	memset(filename, 0, sizeof filename);
	memset(port, 0, sizeof port);
	memset(ipaddr, 0, sizeof ipaddr);
	
	get_input(argv[1], filename, ipaddr, port);
	
	char * Port = PORT;
	if(*port != '\x00') {
		 Port = port;
	}

//	printf("%s, %s, %s", fid, ipaddr, Port);
//	exit(0);

	if ((rv = getaddrinfo(ipaddr, Port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);
	

// **************************
	char * content;
	
	/*
	FILE * http_req = fopen(filename, "rb");
	if(http_req == NULL){
		printf("failure occurred when opening");
	}
	fseek(http_req, 0, SEEK_END);
	int file_len = ftell(http_req);
	rewind(http_req);
	*/
	
	
	char send_buff[1024];
	memset(send_buff, 0, sizeof send_buff);
	sprintf(send_buff, HEAD, filename, ipaddr, Port);
	int total_len = strlen(send_buff);
	/*fread(content, file_len, 1, http_req);
	fclose(http_req);
	*/
	int sent = 0;
	while(sent < total_len){
		int n = send(sockfd, content, total_len - sent, 0);
		if(n == -1){
			printf("failure when sending file");
		}
		sent = sent + n;
	}

	freeaddrinfo(servinfo); // all done with this structure


/*	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}
	buf[numbytes] = '\0';

	printf("client: received '%s'\n",buf);

*/
	int received = 0;
	int n = 1;
	FILE * fp = fopen(filename, "wb");
	while(1){
		n = recv(sockfd, buf, MAXDATASIZE, 0);
		if(n == -1){
			break;
		}
		fwrite(buf, n, 1, fp);
		received += n;
	}
	close(sockfd);
	printf("finished receiving file, the size is: %d Bytes \n", received);
	return 0;
}

