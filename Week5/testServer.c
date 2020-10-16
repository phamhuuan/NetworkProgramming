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

int main() {
	int listenfd, connfd, n;
	pid_t childpid;
	socklen_t clilen;
	char buff[MAX_LINE];
	struct sockaddr_in cliaddr, servaddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	servaddr.sin_family = AF_INET;
	// servaddr.sin_addr.s_addr = htonl(0x030eb6cb);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVER_PORT);
	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	listen(listenfd, LISTENQ);
	printf("Server is running\n");
	for(;;) {
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
		printf("Recieved request\n");
		while ((n = recv(connfd, buff, MAX_LINE, 0)) > 0) {
			printf("String recieved: ");
			puts(buff);
			int a = send(connfd, buff, n, 0);
			printf("%d\n", a);
		}
		if (n < 0) {
			perror("Error");
			exit(1);
		}
		close(connfd);
	}
	close(listenfd);
}