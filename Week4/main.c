#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1000
#define SERV_PORT 4444

int initSocket(int *sockfd);
int startServer(int *sockfd, struct sockaddr_in *servaddr);
int startClient(int *sockfd, struct sockaddr_in *servaddr);
int communicateWithClient(socklen_t *len, int *sockfd, int *rcvBytes, int *sendBytes, char *buff, struct sockaddr_in *servaddr, struct sockaddr_in *cliaddr);
int communicateWithServer(socklen_t *len, int *sockfd, int *rcvBytes, int *sendBytes, char *buff, struct sockaddr_in *servaddr);
int sendMessage();

int main(int argc, char **argv) {
	int *sockfd = (int *)malloc(sizeof(int));
	int *rcvBytes = (int *)malloc(sizeof(int));
	int *sendBytes = (int *)malloc(sizeof(int));
	socklen_t *len = (socklen_t *)malloc(sizeof(socklen_t));
	char *buff = (char *)malloc(sizeof(char) * (BUFF_SIZE + 1));
	struct sockaddr_in *servaddr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
	struct sockaddr_in *cliaddr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
	
	if (strcmp(argv[0], "./server") == 0) {
		if (!initSocket(sockfd)) {
			return 0;
		}
		if (!startServer(sockfd, servaddr)) {
			return 0;
		}
		communicateWithClient(len, sockfd, rcvBytes, sendBytes, buff, servaddr, cliaddr);
	} else if (strcmp(argv[0], "./client") == 0) {
		if (!initSocket(sockfd)) {
			return 0;
		}
		if (!startClient(sockfd, servaddr)) {
			return 0;
		}
		while (1) {
			communicateWithServer(len, sockfd, rcvBytes, sendBytes, buff, servaddr);
		}
		
	}

	free(sockfd);
	free(servaddr);
	free(cliaddr);
	free(len);
	free(buff);
	free(rcvBytes);
	free(sendBytes);
	return 0;
}

int initSocket(int *sockfd) {
	if((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Error");
		return 0;
	}
	return 1;
}

int startServer(int *sockfd, struct sockaddr_in *servaddr) {
	bzero(servaddr, sizeof(*servaddr));
	(*servaddr).sin_family = AF_INET;
	(*servaddr).sin_addr.s_addr = htonl(INADDR_ANY);
	(*servaddr).sin_port = htons(SERV_PORT);
	if (bind(*sockfd, (struct sockaddr *) servaddr, sizeof(*servaddr))) {
		perror("Bind error");
		return 0;
	}
	return 1;
}

int startClient(int *sockfd, struct sockaddr_in *servaddr) {
	bzero(servaddr, sizeof(*servaddr));
	(*servaddr).sin_family = AF_INET;
	(*servaddr).sin_addr.s_addr = inet_addr("18.158.249.75");
	(*servaddr).sin_port = htons(SERV_PORT);
	return 1;
}

int communicateWithClient(socklen_t *len, int *sockfd, int *rcvBytes, int *sendBytes, char *buff, struct sockaddr_in *servaddr, struct sockaddr_in *cliaddr) {
	for( ; ; ) {
		*len = sizeof(*cliaddr);
		*rcvBytes = recvfrom(*sockfd, buff, BUFF_SIZE, 0,(struct sockaddr *) cliaddr, len);
		if (*rcvBytes < 0) {
			perror("Error");
			return 0;
		}
		buff[*rcvBytes] = '\0';
		printf("[%s:%d]: %s", inet_ntoa((*cliaddr).sin_addr), ntohs((*cliaddr).sin_port), buff);
		*sendBytes = sendto(*sockfd, buff, *rcvBytes, 0,(struct sockaddr *) cliaddr, *len);
		if (*sendBytes < 0) {
			perror("Error");
			return 0;
		}
	}
}

int communicateWithServer(socklen_t *len, int *sockfd, int *rcvBytes, int *sendBytes, char *buff, struct sockaddr_in *servaddr) {
	printf("Send to server: ");
	fgets(buff, BUFF_SIZE, stdin);
	*len = sizeof(*servaddr);
	*sendBytes = sendto(*sockfd, buff, strlen(buff), 0,(struct sockaddr *) servaddr, *len);
	if (*sendBytes < 0) {
		perror("Error: ");
		return 0;
	}
	*rcvBytes = recvfrom(*sockfd, buff, BUFF_SIZE, 0, (struct sockaddr *) servaddr, len);
	if (*rcvBytes < 0) {
		perror("Error: ");
		return 0;
	}
	buff[*rcvBytes] = '\0';
	printf("Reply from server: %s", buff);
}