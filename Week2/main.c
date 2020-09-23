#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int hostname_to_ip(char *  , char *);

int main(int argc , char *argv[]) {
	switch (argc)
	{
	case 2:
		
		break;
	
	default:
		break;
	}
	if(argc <2) {
		printf("Please provide a hostname to resolve");
		exit(1);
	}
	
	char *hostname = argv[1];
	char ip[100];
	
	hostname_to_ip(hostname , ip);
	printf("%s resolved to %s" , hostname , ip);
	
	printf("\n");
	
}
/*
	Get ip from domain name
 */

int hostname_to_ip(char *hostname , char *ip) {
	int sockfd;  
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_in *h;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname , "http" , &hints , &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		h = (struct sockaddr_in *) p->ai_addr;
		strcpy(ip , inet_ntoa( h->sin_addr ));
	}
	
	freeaddrinfo(servinfo); // all done with this structure
	return 0;
}