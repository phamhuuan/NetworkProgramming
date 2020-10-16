#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "helper.h"

void writeFile();
void encode(char *, char *);
void decode(char *, char *);
void handleMessage(int *sockfd, char *buff, struct sockaddr_in *addr);

FILE *f;
char *username, *password, *homepage;

int main(int argc, char **argv) {
	if (argc != 2) {
		printError("Syntax error\n")
		return 0;
	}
	char *port = (char *)malloc(sizeof(char) * 10);
	
	sprintf(port, "%d", atoi(argv[1]));
	if (strcmp(port, argv[1]) != 0) {
		printError2("%s is not a valid port number\n", argv[1])
		free(port);
		return 0;
	} else {
		if (!checkValidPort(atoi(port))) {
			printError2("%s is not a valid port number\n", port)
			free(port);
			return 0;
		} else {
			free(port);
		}
	}

	f = fopen(DATA_FILE, "r");
	if (f == NULL) {
		printError("Error: Cannot open file\n")
		return 0;
	} else {
		elementtype *user = (elementtype *)malloc(sizeof(elementtype));
		while (fscanf(f, "%s%s%s", user->username, user->password, user->status) != EOF) {
			insertBefore(*user);
		}
		fclose(f);
		free(user);
	}

	int *sockfd = (int *)malloc(sizeof(int));
	char *buff = (char *)malloc(sizeof(char) * (BUFF_SIZE + 1));
	struct sockaddr_in *servaddr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
	struct sockaddr_in *cliaddr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));

	if (initSocket(sockfd)) {
		if (startServer(sockfd, servaddr, atoi(argv[1]))) {
			while (listenMessage(sockfd, buff, cliaddr)) {
				handleMessage(sockfd, buff, cliaddr);
			}
		} else {
			printError("Something went wrong, please try agian later!\n")
		}
	} else {
		printError("Something went wrong, please try agian later!\n")
	}

	free(sockfd);
	free(servaddr);
	free(cliaddr);
	free(buff);
	return 0;
}

void updateList(char *username, int status) {
	traversingList {
		if (strcmp(p->element.username, username) == 0) {
			char *statusString = (char *)malloc(sizeof(char) * 3);
			sprintf(statusString, "%d", status);
			strcpy(p->element.status, statusString);
			free(statusString);
			break;
		}
	}
}

void updateList2(char *username, char *password) {
	traversingList {
		if (strcmp(p->element.username, username) == 0) {
			strcpy(p->element.password, password);
			break;
		}
	}
}

void handleMessage(int* sockfd, char *buff, struct sockaddr_in *addr) {
	char *token = strtok(buff, "|");
	int *actionCode = (int *)malloc(sizeof(int));
	char a[100], b[100];
	*actionCode = atoi(token);
	elementtype *newUser = (elementtype *)malloc(sizeof(elementtype));
	char *sendBuff;
	switch (*actionCode) {
		case FIND_ACCOUNT:
			// code|username
			temp = find(strtok(NULL, "|"));
			if (temp == NULL) {
				sendBuff = (char *)malloc(sizeof(char) * 3);
				sprintf(sendBuff, "%d", FIND_ACCOUNT_NOT_FOUND);
				sendMessage(sockfd, sendBuff, addr);
				free(sendBuff);
			} else {
				sendBuff = (char *)malloc(sizeof(char) * BUFF_SIZE);
				sprintf(sendBuff, "%d", FIND_ACCOUNT_FOUND);
				strcat(sendBuff, "|");
				strcat(sendBuff, temp->element.username);
				strcat(sendBuff, "|");
				strcat(sendBuff, temp->element.password);
				strcat(sendBuff, "|");
				strcat(sendBuff, temp->element.status);
				sendMessage(sockfd, sendBuff, addr);
				free(sendBuff);
			}
			break;
		case REGISTER:
			strcpy(newUser->username, strtok(NULL, "|"));
			strcpy(newUser->password, strtok(NULL, "|"));
			sprintf(newUser->status, "%d", IDLE);
			insertBefore(*newUser);
			writeFile();
			sendBuff = (char *)malloc(sizeof(char) * 3);
			sprintf(sendBuff, "%d", REGISTER_SUCCESS);
			sendMessage(sockfd, sendBuff, addr);
			free(sendBuff);
			break;
		case BLOCK_ACCOUNT:
			updateList(strtok(NULL, "|"), BLOCKED);
			writeFile();
			sendBuff = (char *)malloc(sizeof(char) * 3);
			sprintf(sendBuff, "%d", BLOCK_ACCOUNT_SUCCESS);
			sendMessage(sockfd, sendBuff, addr);
			free(sendBuff);
			break;
		case ACTIVE_ACCOUNT:
			updateList(strtok(NULL, "|"), ACTIVE);
			writeFile();
			sendBuff = (char *)malloc(sizeof(char) * 3);
			sprintf(sendBuff, "%d", ACTIVE_ACCOUNT_SUCCESS);
			sendMessage(sockfd, sendBuff, addr);
			free(sendBuff);
			break;
		case CHANGE_PASSWORD:
			strcpy(a, strtok(NULL, "|"));
			strcpy(b, strtok(NULL, "|"));
			updateList2(a, b);
			// updateList2(strtok(NULL, "|"), strtok(NULL, "|"));
			writeFile();
			sendBuff = (char *)malloc(sizeof(char) * 3);
			sprintf(sendBuff, "%d", CHANGE_PASSWORD_SUCCESS);
			sendMessage(sockfd, sendBuff, addr);
			free(sendBuff);
			break;
		default:
			break;
	}
	free(newUser);
	free(actionCode);
}

void encode(char *string, char *result) {
	int i, j = 0;
	for (i = 0; string[i] != '\0'; i++) {
		sprintf((char *)(result + j), "%03d", string[i]);
		j+=3;
	}
	result[j] = '\0';
}

void decode(char *string, char *result) {
	int i = 0, j = 0;
	char temp[4];
	for (i = 0; i < strlen(string) - 1; i += 3) {
		temp[0] = string[i];
		temp[1] = string[i+1];
		temp[2] = string[i+2];
		temp[3] = '\0';
		result[j] = atoi(temp);
		j++;
	} 
	result[j] = '\0';
}

void writeFile() {
	f = fopen(DATA_FILE, "w+");
	if (f == NULL) {
		printError("Error: Cannot open file\n")
	} else {
		root = reverseList(root);
		traversingList {
			fprintf(f, "%s %s %s\n", p->element.username, p->element.password, p->element.status);
		}
	}
	fclose(f);
}
