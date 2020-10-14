#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "helper.h"

char *encode(char *, char *);
char *decode(char *, char *);
void registerFunction(int *sockfd, struct sockaddr_in *servaddr);
void activeFunction(int *sockfd, struct sockaddr_in *servaddr);
void loginFunction(int *sockfd, struct sockaddr_in *servaddr);
void searchFunction(int *sockfd, struct sockaddr_in *servaddr);
void changePasswordFunction(int *sockfd, struct sockaddr_in *servaddr);
void logoutFunction();
void visitDomainFunction();
void visitIpFunction();
void exitFunction();

elementtype tempUser, currentUser;
int isLogin = FALSE;

int main(int argc, char **argv) {
	if (argc != 3) {
		printError("Syntax error\n")
		return 0;
	}
	if (strlen(argv[1]) > 15) {
		printError2("%s is not a valid ip address\n", argv[1])
		return 0;
	}
	char *ip = (char *)malloc(sizeof(char) * 16);
	strcpy(ip, argv[1]);
	if (!checkValidIp(ip)) {
		printError2("%s is not a valid ip address\n", argv[1])
		free(ip);
		return 0;
	}
	char *port = (char *)malloc(sizeof(char) * 10);
	
	sprintf(port, "%d", atoi(argv[2]));
	if (strcmp(port, argv[2]) != 0) {
		printError2("%s is not a valid port number\n", argv[2])
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

	int *sockfd = (int *)malloc(sizeof(int));
	char *buff = (char *)malloc(sizeof(char) * (BUFF_SIZE + 1));
	struct sockaddr_in *servaddr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
	struct sockaddr_in *cliaddr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));

	if (initSocket(sockfd)) {
		if (startClient(argv[1], servaddr, atoi(argv[2]))) {
			char *choiceString = (char *)malloc(sizeof(char) * 10);
			while (1) {
				menu(choiceString);
				if (strcmp(choiceString, "0.0") == 0) {
					printError("Syntax error")
					continue;
				}
				int *choiceNumber = (int *)malloc(sizeof(int));
				*choiceNumber = atoi(choiceString);
				switch (*choiceNumber) {
					case 1:
						registerFunction(sockfd, servaddr);
						break;
					case 2:
						activeFunction(sockfd, servaddr);
						break;
					case 3:
						loginFunction(sockfd, servaddr);
						break;
					case 4:
						searchFunction(sockfd, servaddr);
						break;
					case 5:
						changePasswordFunction(sockfd, servaddr);
						break;
					case 6:
						logoutFunction();
						break;
					case 7:
						visitDomainFunction();
						break;
					case 8:
						visitIpFunction();
						break;
					default:
						exitFunction();
						return 0;
				}
				free(choiceNumber);
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

char *encode(char *string, char *result) {
	int i, j = 0;
	for (i = 0; string[i] != '\0'; i++) {
		sprintf((char *)(result + j), "%03d", string[i]);
		j+=3;
	}
	result[j] = '\0';
	return result;
}

char *decode(char *string, char *result) {
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
	return result;
}

void inputUsername(char *username) {
	inputString("Username: ", 30, username);
}

void inputPassword(char *password) {
	char *text;
	while (1) {
		text = getpass("\x1b[0m\x1b[31mPassword: ");
		Reset
		if (strlen(text) <= 0 || strlen(text) > 30) {
			printError2("Error: Password's length must between 1 and %d\n", 30)
		} else {
			break;
		}
	}
	strcpy(password, text);
}

void inputHomepage(char *homepage) {
	char *ip, *domain;
	int *isValidIp, *isValidDomain;
	while (1) {
		inputString("Homepage: ", 100, homepage);
		ip = (char *)malloc(sizeof(char) * 100);
		strcpy(ip, homepage);
		domain = (char *)malloc(sizeof(char) * 100);
		strcpy(domain, homepage);
		isValidIp = (int *)malloc(sizeof(int));
		*isValidIp = checkValidIp(ip);
		isValidDomain = (int *)malloc(sizeof(int));
		*isValidDomain = checkValidDomain(domain);
		if (!*isValidIp && !*isValidDomain) {
			printError2("%s is not a validate homepage\n", homepage);
			continue;
		}
		if (*isValidIp) {
			strcpy(ip, homepage);
			char *result = (char *)malloc(sizeof(char) * 30);
			*isValidIp = ip_to_hostname(result, ip);
			free(result);
			if (!*isValidIp) {
				printError2("Not found infomation for %s\n", homepage);
				continue;
			}
		}
		if (*isValidDomain) {
			strcpy(domain, homepage);
			char *result = (char *)malloc(sizeof(char) * 30);
			*isValidDomain = hostname_to_ip(domain, result);
			free(result);
			if (!*isValidDomain) {
				printError2("Not found infomation for %s\n", homepage);
				continue;
			}
		}
		break;
	}
	free(ip);
	free(domain);
	free(isValidIp);
	free(isValidDomain);
}

int handleMessage(char *buff) {
	char *token = strtok(buff, "|");
	int *actionCode = (int *)malloc(sizeof(int));
	*actionCode = atoi(token);
	int returnValue = *actionCode;
	switch (*actionCode) {
		case FIND_ACCOUNT_FOUND:
			strcpy(tempUser.username, strtok(NULL, "|"));
			strcpy(tempUser.password, strtok(NULL, "|"));
			strcpy(tempUser.status, strtok(NULL, "|"));
			strcpy(tempUser.homepage, strtok(NULL, "|"));
			break;
		default:
			break;
	}
	free(actionCode);
	return returnValue;
}

int checkPassword(int *sockfd, struct sockaddr_in *servaddr, char *password, char *encodePassword, char *encodeUsername) {
	int *timesLeft = (int *)malloc(sizeof(int));
	*timesLeft = MAX_TIMES;
	while (1) {
		inputPassword(password);
		encode(password, encodePassword);
		if (strcmp(encodePassword, tempUser.password) == 0) {
			return TRUE;
		} else {
			*timesLeft = *timesLeft - 1;
			if (*timesLeft == 0) {
				char *sendBuff2 = (char *)malloc(sizeof(char) * BUFF_SIZE);
				sprintf(sendBuff2, "%d", BLOCK_ACCOUNT);
				strcat(sendBuff2, "|");
				strcat(sendBuff2, encodeUsername);
				sendMessage(sockfd, sendBuff2, servaddr);
				free(sendBuff2);
				char *recieveBuff2 = (char *)malloc(sizeof(char) * BUFF_SIZE);
				listenMessage(sockfd, recieveBuff2, servaddr);
				if (handleMessage(recieveBuff2) == BLOCK_ACCOUNT_SUCCESS) {
					printError("Error: Wrong password, your account is blocked\n")
				}
				free(recieveBuff2);
				return FALSE;
			} else {
				printError2("Error: Wrong password! %d time(s) remaining\n", *timesLeft)
			}
		}
	}
}

void checkActivationCode(int *sockfd, struct sockaddr_in *servaddr, char *encodeUsername) {
	int *timesLeft = (int *)malloc(sizeof(int));
	*timesLeft = MAX_TIMES;
	char *activationCode = (char *)malloc(sizeof(char) * 11);
	while (1) {
		inputString("Activation code: ", 10, activationCode);
		if (strcmp(activationCode, ACTIVATION_CODE) == 0) {
			char *sendBuff2 = (char *)malloc(sizeof(char) * BUFF_SIZE);
			sprintf(sendBuff2, "%d", ACTIVE_ACCOUNT);
			strcat(sendBuff2, "|");
			strcat(sendBuff2, encodeUsername);
			sendMessage(sockfd, sendBuff2, servaddr);
			free(sendBuff2);
			char *recieveBuff2 = (char *)malloc(sizeof(char) * BUFF_SIZE);
			listenMessage(sockfd, recieveBuff2, servaddr);
			if (handleMessage(recieveBuff2) == ACTIVE_ACCOUNT_SUCCESS) {
				printSuccess("Activate account success\n")
			}
			free(recieveBuff2);
			break;
		} else {
			*timesLeft = *timesLeft - 1;
			if (*timesLeft == 0) {
				char *sendBuff2 = (char *)malloc(sizeof(char) * BUFF_SIZE);
				sprintf(sendBuff2, "%d", BLOCK_ACCOUNT);
				strcat(sendBuff2, "|");
				strcat(sendBuff2, encodeUsername);
				sendMessage(sockfd, sendBuff2, servaddr);
				free(sendBuff2);
				char *recieveBuff2 = (char *)malloc(sizeof(char) * BUFF_SIZE);
				listenMessage(sockfd, recieveBuff2, servaddr);
				if (handleMessage(recieveBuff2) == BLOCK_ACCOUNT_SUCCESS) {
					printError("Error: Wrong activation, your account is blocked\n")
				}
				free(recieveBuff2);
				break;
			} else {
				printError2("Error: Wrong activation! %d time(s) remaining\n", *timesLeft)
			}
		}
	}
	free(activationCode);
}

void registerFunction(int *sockfd, struct sockaddr_in *servaddr) {
	if (isLogin) {
		char *tempUsername = (char *)malloc(sizeof(char) * 31);
		decode(tempUser.username, tempUsername);
		printWarning2("Already logged in as %s\nYou have to log out to create new account\n", tempUsername)
		free(tempUsername);
		return;
	}
	char *username = (char *)malloc(sizeof(char) * 31);
	char *password = (char *)malloc(sizeof(char) * 31);
	char *homepage = (char *)malloc(sizeof(char) * 100);
	char *encodeUsername = (char *)malloc(sizeof(char) * 100);
	char *encodePassword = (char *)malloc(sizeof(char) * 100);
	inputUsername(username);
	char *sendBuff = (char *)malloc(sizeof(char) * 1000);
	strcpy(sendBuff, "");
	sprintf(sendBuff, "%d", FIND_ACCOUNT);
	strcat(sendBuff, "|");
	strcat(sendBuff, encode(username, encodeUsername));
	if (sendMessage(sockfd, sendBuff, servaddr)) {
		free(sendBuff);
		char *recieveBuff = (char *)malloc(sizeof(char) * BUFF_SIZE);
		if (listenMessage(sockfd, recieveBuff, servaddr)) {
			switch (handleMessage(recieveBuff)) {
				case FIND_ACCOUNT_NOT_FOUND:
					inputPassword(password);
					inputHomepage(homepage);
					char *sendBuff2 = (char *)malloc(sizeof(char) * BUFF_SIZE);
					sprintf(sendBuff2, "%d", REGISTER);
					strcat(sendBuff2, "|");
					strcat(sendBuff2, encodeUsername);
					strcat(sendBuff2, "|");
					strcat(sendBuff2, encode(password, encodePassword));
					strcat(sendBuff2, "|");
					strcat(sendBuff2, homepage);
					sendMessage(sockfd, sendBuff2, servaddr);
					free(sendBuff2);
					char *recieveBuff2 = (char *)malloc(sizeof(char) * BUFF_SIZE);
					if (listenMessage(sockfd, recieveBuff2, servaddr)) {
						if (handleMessage(recieveBuff2) == REGISTER_SUCCESS) {
							printSuccess("Register success\n")
						}
					}
					free(recieveBuff2);
					break;
				case FIND_ACCOUNT_FOUND:
					printError2("Error: Account for username \"%s\" already exists!\n", username)
					break;
			}
			free(recieveBuff);
		}
	} else {
		free(sendBuff);
	}
	
	free(username);
	free(password);
	free(homepage);
	free(encodeUsername);
	free(encodePassword);
}

void activeFunction(int *sockfd, struct sockaddr_in *servaddr) {
	if (isLogin) {
		char *tempUsername = (char *)malloc(sizeof(char) * 31);
		decode(tempUser.username, tempUsername);
		printWarning2("Already logged in as %s\nYou have to log out to active another account\n", tempUsername)
		free(tempUsername);
		return;
	}
	char *username = (char *)malloc(sizeof(char) * 31);
	char *password = (char *)malloc(sizeof(char) * 31);
	char *encodeUsername = (char *)malloc(sizeof(char) * 100);
	char *encodePassword = (char *)malloc(sizeof(char) * 100);
	inputUsername(username);
	char *sendBuff = (char *)malloc(sizeof(char) * 1000);
	strcpy(sendBuff, "");
	sprintf(sendBuff, "%d", FIND_ACCOUNT);
	strcat(sendBuff, "|");
	strcat(sendBuff, encode(username, encodeUsername));
	if (sendMessage(sockfd, sendBuff, servaddr)) {
		free(sendBuff);
		char *recieveBuff = (char *)malloc(sizeof(char) * BUFF_SIZE);
		if (listenMessage(sockfd, recieveBuff, servaddr)) {
			switch (handleMessage(recieveBuff)) {
				case FIND_ACCOUNT_NOT_FOUND:
					printError2("Error: Account for username \"%s\" does not exists!\n", username)
					break;
				case FIND_ACCOUNT_FOUND:
					if (!checkPassword(sockfd, servaddr, password, encodePassword, encodeUsername)) {
						break;
					}
					if (atoi(tempUser.status) == BLOCKED) {
						printError("This account has been blocked!\n")
					} else if (atoi(tempUser.status) == ACTIVE) {
						printWarning("This account has been activated!\n")
					} else {
						checkActivationCode(sockfd, servaddr, encodeUsername);
					}
					break;
			}
		}
		free(recieveBuff);
	} else {
		free(sendBuff);
	}
	
	free(username);
	free(password);
	free(encodeUsername);
	free(encodePassword);
}

void loginFunction(int *sockfd, struct sockaddr_in *servaddr) {
	if (isLogin) {
		char *tempUsername = (char *)malloc(sizeof(char) * 31);
		decode(tempUser.username, tempUsername);
		printWarning2("Already logged in as %s\nYou have to log out to log in another account\n", tempUsername)
		free(tempUsername);
		return;
	}
	char *username = (char *)malloc(sizeof(char) * 31);
	char *password = (char *)malloc(sizeof(char) * 31);
	char *encodeUsername = (char *)malloc(sizeof(char) * 100);
	char *encodePassword = (char *)malloc(sizeof(char) * 100);
	inputUsername(username);
	char *sendBuff = (char *)malloc(sizeof(char) * 1000);
	strcpy(sendBuff, "");
	sprintf(sendBuff, "%d", FIND_ACCOUNT);
	strcat(sendBuff, "|");
	strcat(sendBuff, encode(username, encodeUsername));
	if (sendMessage(sockfd, sendBuff, servaddr)) {
		free(sendBuff);
		char *recieveBuff = (char *)malloc(sizeof(char) * BUFF_SIZE);
		if (listenMessage(sockfd, recieveBuff, servaddr)) {
			switch (handleMessage(recieveBuff)) {
				case FIND_ACCOUNT_NOT_FOUND:
					printError2("Error: Account for username \"%s\" does not exists!\n", username)
					break;
				case FIND_ACCOUNT_FOUND:
					if (!checkPassword(sockfd, servaddr, password, encodePassword, encodeUsername)) {
						break;
					}
					if (atoi(tempUser.status) == BLOCKED) {
						printError("This account has been blocked!\n")
					} else if (atoi(tempUser.status) == IDLE) {
						printError("This account has not been activated\n")
					} else {
						isLogin = TRUE;
						currentUser = tempUser;
						printSuccess("Login success\n")
					}
					break;
			}
		}
		free(recieveBuff);
	} else {
		free(sendBuff);
	}
	
	free(username);
	free(password);
	free(encodeUsername);
	free(encodePassword);
}

char *convertStatus(int status) {
	switch (status)
	{
	case BLOCKED:
		return "blocked";
	case IDLE:
		return "idle";
	case ACTIVE:
		return "active";
	default:
		return "undefined";
	}
}

void searchFunction(int *sockfd, struct sockaddr_in *servaddr) {
	char *username = (char *)malloc(sizeof(char) * 31);
	char *encodeUsername = (char *)malloc(sizeof(char) * 100);
	inputUsername(username);
	char *sendBuff = (char *)malloc(sizeof(char) * 1000);
	strcpy(sendBuff, "");
	sprintf(sendBuff, "%d", FIND_ACCOUNT);
	strcat(sendBuff, "|");
	strcat(sendBuff, encode(username, encodeUsername));
	if (sendMessage(sockfd, sendBuff, servaddr)) {
		free(sendBuff);
		char *recieveBuff = (char *)malloc(sizeof(char) * BUFF_SIZE);
		if (listenMessage(sockfd, recieveBuff, servaddr)) {
			char *tempUsername;
			switch (handleMessage(recieveBuff)) {
				case FIND_ACCOUNT_NOT_FOUND:
					printError2("Error: Account for username \"%s\" does not exists!\n", username)
					break;
				case FIND_ACCOUNT_FOUND:
					tempUsername = (char *)malloc(sizeof(char) * 31);
					decode(tempUser.username, tempUsername);
					FgGreen
					printf("Username: %s\nStatus: %s\n", tempUsername, convertStatus(atoi(tempUser.status)));
					Reset
					break;
			}
		}
		free(recieveBuff);
	} else {
		free(sendBuff);
	}
	free(username);
	free(encodeUsername);
}

void changePasswordFunction(int *sockfd, struct sockaddr_in *servaddr) {
	if (!isLogin) {
		printError("Error: You have not logged in\n")
		return;
	}
	char *password = (char *)malloc(sizeof(char) * 31);
	char *encodePassword = (char *)malloc(sizeof(char) * 100);
	char *newPassword = (char *)malloc(sizeof(char) * 31);
	char *encodeNewPassword = (char *)malloc(sizeof(char) * 100);
	if (checkPassword(sockfd, servaddr, password, encodePassword, currentUser.username)) {
		while(1) {
			inputPassword(newPassword);
			encode(newPassword, encodeNewPassword);
			if (strcpy(encodePassword, encodeNewPassword) == 0) {
				printError("Error: The new password must be different from the old one!\n")
			} else {
				char *sendBuff = (char *)malloc(sizeof(char) * 100);
				sprintf(sendBuff, "%d", CHANGE_PASSWORD);
				strcat(sendBuff, "|");
				strcat(sendBuff, currentUser.username);
				strcat(sendBuff, "|");
				strcat(sendBuff, encodeNewPassword);
				sendMessage(sockfd, sendBuff, servaddr);
				free(sendBuff);
				char *recieveBuff = (char *)malloc(sizeof(char) * BUFF_SIZE);
				if (listenMessage(sockfd, recieveBuff, servaddr)) {
					if (handleMessage(recieveBuff) == CHANGE_PASSWORD_SUCCESS) {
						printSuccess("Change password success\n");
					}
				}
				free(recieveBuff);
				break;
			}
		}
	}
	free(password);
	free(encodePassword);
	free(newPassword);
	free(encodeNewPassword);
}

void logoutFunction() {
	if (!isLogin) {
		printError("Error: You have not logged in!\n")
		return;
	}
	isLogin = FALSE;
	printSuccess("Logout success!\n")
}

void visitIpFunction() {
	if (!isLogin) {
		printError("Error: You have not logged in\n")
		return;
	}
	char *ip = (char *)malloc(sizeof(char) * 30);
	char *domain = (char *)malloc(sizeof(char) * 100);
	strcpy(domain, currentUser.homepage);
	hostname_to_ip(domain, ip);
	printSuccess2("Ip: %s\n", ip);
	free(ip);
	free(domain);
}

void visitDomainFunction() {
	if (!isLogin) {
		printError("Error: You have not logged in\n")
		return;
	}
	char *ip = (char *)malloc(sizeof(char) * 30);
	char *domain = (char *)malloc(sizeof(char) * 100);
	strcpy(ip, currentUser.homepage);
	ip_to_hostname(domain, ip);
	printSuccess2("Domain: %s\n", ip);
	free(ip);
	free(domain);
}

void exitFunction() {
	FgCyan
	printf("Thanks for using my application!\n");
	printf("Visit me at github: https://github.com/phamhuuan/NetworkProgramming\n");
	Reset
}
