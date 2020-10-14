#include "helper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int checkValidPort(const unsigned int SERVER_PORT) {
	return SERVER_PORT > 0 && SERVER_PORT < 9999;
}

int ip_to_hostname(char *hostname, char *ip) {
	int rv;
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(ip);
	serverAddress.sin_port = htons(3000);
	if ((rv = getnameinfo((struct sockaddr *) &serverAddress, sizeof (struct sockaddr), hostname, NI_MAXHOST, NULL, 0, NI_NAMEREQD)) != 0) {
		return 0;
	}
	return 1;
}

int hostname_to_ip(char *hostname , char *ip) {
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_in *h;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname , "http" , &hints , &servinfo)) != 0) {
		return 0;
	}
	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		h = (struct sockaddr_in *) p->ai_addr;
		strcpy(ip , inet_ntoa(h->sin_addr));
	}

	char *hostname2 = (char *)malloc(sizeof(char *) * NI_MAXHOST);

	if (ip_to_hostname(hostname2, ip) == 0) {
		free(hostname2);
		return 0;
	} else {
		free(hostname2);
		return 1;
	}
	return 1;
}

int checkValidIp(char *ip) {
	int count = 0;
	char *token = strtok(ip, ".");
	while(token != NULL) {
		count++;
		if (count > 4) {
			return FALSE;
		}
		int *tmp = (int *)malloc(sizeof(int));
		*tmp = atoi(token);
		if (*tmp > 255 || *tmp < 0) {
			return FALSE;
		}
		char *tmp2 = (char *)malloc(sizeof(char) * 20);
		sprintf(tmp2, "%d", *tmp);
		if (strcmp(tmp2, token) != 0) {
			return FALSE;
		}
		free(tmp2);
		free(tmp);
		token = strtok(NULL, ".");
	}
	if (count < 4) {
		return FALSE;
	}
	return TRUE;
}


int isNumber(char c) {
	if (c >= '0' && c <= '9') {
		return 1;
	}
	return 0;
}

int isLetter(char c) {
	if (c >= 'a' && c <= 'z') {
		return 1;
	}
	if (c >= 'A' && c <= 'Z') {
		return 1;
	}
	return 0;
}

int isDot(char c) {
	return c == '.';
}

int isDash(char c) {
	return c == '-';
}

int isSubValid(char *string) {
	int length = strlen(string);
	if (isLetter(string[0]) || isNumber(string[0])) {
		if (isLetter(string[length - 1]) || isNumber(string[length - 1])) {
			for (int i = 1; i < length - 1; i++) {
				if (!isLetter(string[i]) && !isNumber(string[i]) && !isDash(string[i])) {
					return 0;
				}
			}
			return 1;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

int checkValidDomain(char *domain) {
	int length = strlen(domain);
	int count = 0;
	for (int i = 1; i < length - 1; i++) {
		if (isDot(domain[i])) {
			count++;
		}
		if (isDot(domain[i]) && isDot(domain[i - 1])) {
			return FALSE;
		}
	}
	if (count == 0) return FALSE;
	char *token = strtok(domain, ".");
	if (token == NULL) {
		return FALSE;
	}
	if (!isSubValid(token)) {
		return FALSE;
	}
	if (strlen(token) < 3 || strlen(token) > 63) {
		return FALSE;
	}
	char *subToken = (char*)malloc(sizeof(char) * 64);
	while (1) {
		token = strtok(NULL, ".");
		if (token == NULL) {
			break;
		}
		strcpy(subToken, token);
	}
	if (strlen(subToken) < 2 || strlen(subToken) > 6) {
		return FALSE;
	}
	for (int i = 0; i < strlen(subToken) - 1; i++) {
		if (isDash(subToken[i])) {
			return FALSE;
		}
	}
	return TRUE;
}

int initSocket(int *sockfd) {
	if((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Error");
		return FALSE;
	}
	return TRUE;
}

int startServer(int *sockfd, struct sockaddr_in *servaddr, const unsigned int SERVER_PORT) {
	bzero(servaddr, sizeof(*servaddr));
	(*servaddr).sin_family = AF_INET;
	(*servaddr).sin_addr.s_addr = htonl(INADDR_ANY);
	(*servaddr).sin_port = htons(SERVER_PORT);
	if (bind(*sockfd, (struct sockaddr *) servaddr, sizeof(*servaddr))) {
		perror("Bind error");
		return FALSE;
	}
	return TRUE;
}

int startClient(char *ip, struct sockaddr_in *servaddr, const unsigned int SERVER_PORT) {
	bzero(servaddr, sizeof(*servaddr));
	(*servaddr).sin_family = AF_INET;
	(*servaddr).sin_addr.s_addr = inet_addr(ip);
	(*servaddr).sin_port = htons(SERVER_PORT);
	return 1;
}

int listenMessage(int *sockfd, char *buff, struct sockaddr_in *addr) {
	socklen_t *len = (socklen_t *)malloc(sizeof(socklen_t));
	*len = sizeof(*addr);
	int *rcvBytes = (int *)malloc(sizeof(int));
	*rcvBytes = recvfrom(*sockfd, buff, BUFF_SIZE, 0,(struct sockaddr *) addr, len);
	free(len);
	if (*rcvBytes < 0) {
		perror("Error");
		free(rcvBytes);
		return FALSE;
	}
	buff[*rcvBytes] = '\0';
	free(rcvBytes);
	// printf("[%s:%d]: %s\n", inet_ntoa((*addr).sin_addr), ntohs((*addr).sin_port), buff);
	return TRUE;
}

int sendMessage(int *sockfd, char *buff, struct sockaddr_in *addr) {
	socklen_t *len = (socklen_t *)malloc(sizeof(socklen_t));
	*len = sizeof(*addr);
	int *sendBytes = (int *)malloc(sizeof(int));
	*sendBytes = sendto(*sockfd, buff, sizeof(char) * strlen(buff), 0,(struct sockaddr *) addr, *len);
	free(len);
	if (*sendBytes < 0) {
		perror("Error");
		free(sendBytes);
		return FALSE;
	} else {
		free(sendBytes);
		return TRUE;
	}
}