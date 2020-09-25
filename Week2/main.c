#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "helper.h"

int hostname_to_ip(char *  , char *);
int ip_to_hostname(char *, char *);
int checkValidIp(char *);
void printHelper();
void printVersion();
void printContact();
void printSourceCode();

int main(int argc , char *argv[]) {
	switch (argc) {
	case 2:
		if (strcmp(argv[1], HELP_TAG) == 0 || strcmp(argv[1], HELP_TAG2) == 0) {
			printHelper();
		} else if (strcmp(argv[1], VERSION_TAG) == 0 || strcmp(argv[1], VERSION_TAG2) == 0) {
			printVersion();
		} else if (strcmp(argv[1], CONTACT_TAG) == 0) {
			printContact();
		} else if (strcmp(argv[1], SOURCE_CODE_TAG) == 0) {
			printSourceCode();
		} else {
			printError3("Syntax error. Please try %s or %s to learn more\n", SHOW_USAGE, SHOW_USAGE2)
		}
		return 0;
	case 3:
		if (strcmp(argv[1], TYPE_DOMAIN) == 0) {
			char *ip = (char *)malloc(sizeof(char) * 20);
			if (hostname_to_ip(argv[2], ip)) {
				printf("%s: Official IP: %s\n" , argv[2] , ip);
			}
			free(ip);
		} else if (strcmp(argv[1], TYPE_IP_ADDRESS) == 0) {
			char *hostname = (char *)malloc(sizeof(char) * NI_MAXHOST);
			if (ip_to_hostname(hostname, argv[2])) {
				printf("%s: Official name: %s\n", argv[2], hostname);
			}
			free(hostname);
		} else {
			printError3("Syntax error. Please try %s or %s to learn more\n", SHOW_USAGE, SHOW_USAGE2)
		}
		return 0;
	default:
		printError3("Syntax error. Please try %s or %s to learn more\n", SHOW_USAGE, SHOW_USAGE2)
		return 0;
	}
}

int ip_to_hostname(char *hostname, char *ip) {
	int rv;
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(ip);
	serverAddress.sin_port = htons(3000);
	if ((rv = getnameinfo((struct sockaddr *) &serverAddress, sizeof (struct sockaddr), hostname, NI_MAXHOST, NULL, 0, NI_NAMEREQD)) != 0) {
		printError("Not found information\n")
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
		printError("Not found information\n")
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
}

void printHelper() {
	printf("Usage\n");
	printf("Use with tags:\n");
	printTagUsage(HELP_TAG, "Usage")
	printTagUsage(HELP_TAG2, "")

	printTagUsage(VERSION_TAG, "Version")
	printTagUsage(VERSION_TAG2, "")

	printTagUsage(CONTACT_TAG, "How to contact me")

	printTagUsage(SOURCE_CODE_TAG, "How to find my source code")

	printf("Use with parameters:\n");
	printf("%s 1 <%s>\n", RUN_BIN_FILE, "ip address");
	printf("%s 2 <%s>\n", RUN_BIN_FILE, "domain");
}

void printVersion() {
	printf("Version 20.09.24\n");
	printf("Last update: %s\n", UPDATE_DATE);
}

void printContact() {
	printf("Just for fun :> nothing here\n");
}

void printSourceCode() {
	printf("Visit me at https://github.com/phamhuuan/NetworkProgramming\n");
}