#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_LINE 4096
#define SERVER_PORT 4444
#define LISTENQ 10

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in servaddr;
	char sendline[MAX_LINE], recieveLine[MAX_LINE];
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Problem in creating the socket");
		exit(2);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(SERVER_PORT);
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("Problem in connect");
		exit(3);
	}
	while(fgets(sendline, MAX_LINE, stdin) != NULL) {
		send(sockfd, sendline, MAX_LINE, 0);
		if (recv(sockfd, recieveLine, MAX_LINE, 0) == 0) {
			perror("The server terminated prematurely");
			exit(4);
		}
		puts(recieveLine);
	}
}