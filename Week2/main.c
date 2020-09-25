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
int checkValidDomain(char *);
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

int checkValidIp(char *ip) {
	int count = 0;
	char *token = strtok(ip, ".");
	while(token != NULL) {
		count++;
		if (count > 4) {
			return 0;
		}
		int *tmp = (int *)malloc(sizeof(int));
		*tmp = atoi(token);
		if (*tmp > 255 || *tmp < 0) {
			return 0;
		}
		char *tmp2 = (char *)malloc(sizeof(char) * 20);
		sprintf(tmp2, "%d", *tmp);
		if (strcmp(tmp2, token) != 0) {
			return 0;
		}
		free(tmp2);
		free(tmp);
		token = strtok(NULL, ".");
	}
	if (count < 4) {
		return 0;
	}
	return 1;
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
	for (int i = 1; i < length - 1; i++) {
		if (isDot(domain[i]) && isDot(domain[i - 1])) {
			return 0;
		}
	}
	char *token = strtok(domain, ".");
	if (token == NULL) {
		return 0;
	}
	if (!isSubValid(token)) {
		return 0;
	}
	if (strlen(token) < 3 || strlen(token) > 63) {
		return 0;
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
		return 0;
	}
	for (int i = 0; i < strlen(subToken) - 1; i++) {
		if (isDash(subToken[i])) {
			return 0;
		}
	}
	return 1;
}

int ip_to_hostname(char *hostname, char *ip) {
	char *tmpIp = (char *)malloc(sizeof(char) * 20);
	strcpy(tmpIp, ip);
	if (!checkValidIp(tmpIp)) {
		printError2("%s is not a valid ip address\n", ip);
		free(tmpIp);
		return 0;
	}
	free(tmpIp);
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
	char *tmpHostname = (char *)malloc(sizeof(char) * NI_MAXHOST);
	strcpy(tmpHostname, ip);
	if (!checkValidDomain(tmpHostname)) {
		printError2("%s is not a valid domain\n", hostname)
		free(tmpHostname);
		return 0;
	}
	free(tmpHostname);
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_in *h;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname , "http" , &hints , &servinfo)) != 0) {
		printError("Not found information\n")
		// fprintf(stderr, "%s\n", gai_strerror(rv));
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