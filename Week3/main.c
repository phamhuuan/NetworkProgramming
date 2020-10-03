#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "helper.h"

FILE *f;
char *username, *password, *homepage;
int *isLogin;

int checkValidIp(char *);
int checkValidDomain(char *);
int hostname_to_ip(char *  , char *);
int ip_to_hostname(char *, char *);

int main() {
	f = fopen(DATA_FILE, "r");
	if (f == NULL) {
		printError("Error: Cannot open file\n")
		return 0;
	} else {
		elementtype *user = (elementtype *)malloc(sizeof(elementtype));
		while (fscanf(f, "%s%s%d%s", user->username, user->password, &user->status, user->homepage) != EOF) {
			char *tmpUsername = (char *)malloc(sizeof(char) * 300);
			strcpy(tmpUsername, user->username);
			decode(tmpUsername, user->username);
			free(tmpUsername);
			char *tmpPassword = (char *)malloc(sizeof(char) * 300);
			strcpy(tmpPassword, user->password);
			decode(tmpPassword, user->password);
			free(tmpPassword);
			insertBefore(*user);
		}
		fclose(f);
		free(user);
	}
	char *menuChoice;
	menuChoice = (char *)malloc(sizeof(char) * 30);
	if (menuChoice == NULL) {
		printError("Error: Memory allocation failed!\n")
		return 0;
	}
	int *choice = (int *)malloc(sizeof(int));
	username = (char *)malloc(sizeof(char) * 30);
	password = (char *)malloc(sizeof(char) * 30);
	homepage = (char *)malloc(sizeof(char) * 100);
	isLogin = (int *)malloc(sizeof(int));
	*isLogin = FALSE;
openMenu:
	menu(menuChoice);
	if (strcmp(menuChoice, "Error") == 0) {
		printError("Error: Please try again\n")
		goto openMenu;
	}
	*choice = atoi(menuChoice);
	switch (*choice) {
	case 1:
		registerFunction();
		break;
	case 2:
		activeFunction();
		break;
	case 3:
		loginFunction();
		break;
	case 4:
		searchFunction();
		break;
	case 5:
		changePasswordFunction();
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
	char *menuChoice2;
	menuChoice2 = (char *)malloc(sizeof(char) * 30);
	if (menuChoice2 == NULL) {
		printError("Error: Memory allocation failed!\n")
		return 0;
	}
	int *choice2 = (int *)malloc(sizeof(int));
openMenu2:
	menu2(menuChoice2);
	if (strcmp(menuChoice2, "Error") == 0) {
		printError("Error: Please try again\n")
		goto openMenu2;
	}
	*choice2 = atoi(menuChoice2);
	switch (*choice2) {
	case 1:
		free(menuChoice2);
		free(choice2);
		goto openMenu;
		break;
	case 2:
		free(menuChoice2);
		free(menuChoice);
		free(choice2);
		free(choice);
		exitFunction();
		return 0;
	default:
		goto openMenu2;
		break;
	}
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
			char *tmpUsername = (char *)malloc(sizeof(char) * 30);
			strcpy(tmpUsername, p->element.username);
			encode(tmpUsername, p->element.username);
			char *tmpPassword = (char *)malloc(sizeof(char) * 30);
			strcpy(tmpPassword, p->element.password);
			encode(tmpPassword, p->element.password);
			fprintf(f, "%s %s %d %s\n", p->element.username, p->element.password, p->element.status, p->element.homepage);
			strcpy(tmpUsername, p->element.username);
			decode(tmpUsername, p->element.username);
			strcpy(tmpPassword, p->element.password);
			decode(tmpPassword, p->element.password);
			free(tmpUsername);
			free(tmpPassword);
		}
	}
	fclose(f);
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
	int count = 0;
	for (int i = 1; i < length - 1; i++) {
		if (isDot(domain[i])) {
			count++;
		}
		if (isDot(domain[i]) && isDot(domain[i - 1])) {
			return 0;
		}
	}
	if (count == 0) return 0;
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

void updateList(char *username, int status) {
	traversingList {
		if (strcmp(p->element.username, username) == 0) {
			p->element.status = status;
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

void inputUsername() {
	inputString("Username: ", 30, username);
}

void inputPassword() {
inputPassword:
	password = getpass("\x1b[0m\x1b[31mPassword: ");
	Reset
	if (strlen(password) <= 0 || strlen(password) > 30) {
		printError2("Error: Password's length must between 1 and %d\n", 30)
		goto inputPassword;
	}
}

void inputHomepage() {
inputHomepage:
	inputString("Homepage: ", 100, homepage);
	char *ip = (char *)malloc(sizeof(char) * 100);
	strcpy(ip, homepage);
	char *domain = (char *)malloc(sizeof(char) * 100);
	strcpy(domain, homepage);
	int *isValidIp = (int *)malloc(sizeof(int));
	*isValidIp = checkValidIp(ip);
	int *isValidDomain = (int *)malloc(sizeof(int));
	*isValidDomain = checkValidDomain(domain);
	if (!*isValidIp && !*isValidDomain) {
		printError2("%s is not a validate homepage\n", homepage);
		goto inputHomepage;
	}
	if (*isValidIp) {
		strcpy(ip, homepage);
		char *result = (char *)malloc(sizeof(char) * 30);
		*isValidIp = ip_to_hostname(result, ip);
		free(result);
		if (!*isValidIp) {
			printError2("Not found infomation for %s\n", homepage);
			goto inputHomepage;
		}
	}
	if (*isValidDomain) {
		strcpy(domain, homepage);
		char *result = (char *)malloc(sizeof(char) * 30);
		*isValidDomain = hostname_to_ip(domain, result);
		free(result);
		if (!*isValidDomain) {
			printError2("Not found infomation for %s\n", homepage);
			goto inputHomepage;
		}
	}
	free(ip);
	free(domain);
	free(isValidIp);
	free(isValidDomain);
}

int checkExistAccount() {
	temp = find(username);
	if (temp != NULL) return TRUE;
	return FALSE;
}

int checkIsLoggedIn() {
	// kiem tra bien isLogin
	return *isLogin;
}

int checkPassword() {
	int *timeInputPasswordRemaining;
	timeInputPasswordRemaining = (int *)malloc(sizeof(int));
	*timeInputPasswordRemaining = MAX_TIMES;
checkPassword:
	if (strcmp(temp->element.password, password) != 0) {
		// neu sai mat khau
		if (--*timeInputPasswordRemaining < 1) {
			// neu nhap sai mat khau qua so lan thi tai khoan se bi block va cap nhat lai vao file
			printError("Error: Wrong password, your account is blocked\n")
			updateList(username, BLOCKED);
			writeFile();
			free(timeInputPasswordRemaining);
			return FALSE;
		} else {
			// neu chua nhap qua so lan thi yeu cau nhap lai
			printError2("Error: Wrong password! %d time(s) remaining\n", *timeInputPasswordRemaining)
			inputPassword();
			goto checkPassword;
		}	
	} else {
		// neu dung password
		free(timeInputPasswordRemaining);
		return TRUE;
	}
}

void checkActivationCode() {
	char *activationCode;
	int *timeInputActivationCodeRemaining;
	activationCode = (char *)malloc(sizeof(char) * 10);
	timeInputActivationCodeRemaining = (int *)malloc(sizeof(int));
	*timeInputActivationCodeRemaining = MAX_TIMES;
	// yeu cau nguoi dung nhap ma kich hoat
	inputString("Activation code: ", 10, activationCode);
checkActivationCode:
	// kiem tra ma kich hoat
	if (strcmp(activationCode, ACTIVATION_CODE) != 0) {
		// neu ma kich hoat sai
		if (--*timeInputActivationCodeRemaining < 1) {
			// neu da sai qua MAX_TIMES thi block tai khoan va ghi lai vao file
			printError("Error: Wrong activation code, your account is blocked\n")
			updateList(username, BLOCKED);
			writeFile();
			free(activationCode);
		} else {
			// neu chua sai qua so lan thi yeu cau nhap lai va hien thi so lan nhap con lai
			printError2("Error: Wrong activation code! %d time(s) remaining\n", *timeInputActivationCodeRemaining)
			inputString("Activation code: ", 10, activationCode);
			goto checkActivationCode;
		}
	} else {
		// neu ma kich hoat dung thi kich hoat tai khoan va cap nhat lai vao file
		free(timeInputActivationCodeRemaining);
		printSuccess("Activate account success\n")
		updateList(username, ACTIVE);
		writeFile();
		free(activationCode);
	}
}

// chuyen trang thai tu so sang chu
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

void registerFunction () {
	// kiem tra xem hien tai da login chua, neu chua login moi duoc dang ki
	if (checkIsLoggedIn()) {
		printWarning2("Already logged in as %s\nYou have to log out to create new account\n", username)
		return;
	}
	// nhap username va password
	inputUsername();
	inputPassword();
	// kiem tra xem tai khoan da ton tai hay chua
	if (checkExistAccount()) {
		printError2("Error: Account for username \"%s\" already exists!\n", username)
	} else {
		// neu chua ton tai thi tao tai khoan moi, ghi lai ra file
		elementtype *user = (elementtype *)malloc(sizeof(elementtype));
		strcpy(user->username, username);
		strcpy(user->password, password);
		user->status = IDLE;
		inputHomepage();
		strcpy(user->homepage, homepage);
		insertBefore(*user);
		free(user);
		printSuccess("Register success!\n")
		writeFile();
	}
}

void activeFunction() {
	// kiem tra xem hien tai da login chua, neu chua login moi duoc active tai khoan khac
	if (checkIsLoggedIn()) {
		printWarning2("Already logged in as %s\nYou have to log out to active another account\n", username)
		return;
	}
	// nhap username va password
	inputUsername();
	inputPassword();
	// kiem tra tai co ton tai hay khong
	if (!checkExistAccount()) {
		printError2("Error: Account for username \"%s\" does not exist!\n", username)
	} else {
		// neu tai khoan ton tai thi kiem tra mat khau xem co dung khong
		if (checkPassword()) {
			// neu dung va tai khoan da duoc active
			if (temp->element.status == ACTIVE) {
				printWarning("This account has been activated!\n")
			} else if (temp->element.status == BLOCKED) {
				// neu tai khoan da bi block
				printError("This account has been blocked!\n");
			} else {
				// neu tai khoan dang o trang thai idle thi kiem tra ma kich hoat
				checkActivationCode();
			}
		}
	}
}

void loginFunction() {
	if (checkIsLoggedIn()) {
		printWarning2("Already logged in as %s\nYou have to log out to log in another account\n", username)
		return;
	}
	inputUsername();
	inputPassword();
	if(!checkExistAccount()) {
		printError2("Error: Account for username \"%s\" does not exist!\n", username)
	} else {
		if (checkPassword()) {
			if (temp->element.status == IDLE) {
				printError("This account has not been activated\n")
			} else if (temp->element.status == BLOCKED) {
				printError("This account has been blocked\n")
			} else {
				*isLogin = TRUE;
				printSuccess("Login success\n")
			}
		}
	}
}

void searchFunction() {
	char *searchUsername = (char *)malloc(sizeof(char) * 30);
	inputString("Username: ", 30, searchUsername);
	node *x = find(searchUsername);
	if (x == NULL) {
		printError2("Error: Account for username \"%s\" does not exist!\n", searchUsername)
	} else {
		FgGreen
		printf("Username: %s\nStatus: %s\n", x->element.username, convertStatus(x->element.status));
		Reset
	}
	free(searchUsername);
}

void changePasswordFunction() {
	if (!checkIsLoggedIn()) {
		printError("Error: You have not logged in\n")
		return;
	}
	temp = find(username);
	inputPassword();
	if (checkPassword()) {
		char *newPassword = (char *)malloc(sizeof(char) * 30);
	inputNewPassword:
		newPassword = getpass("\x1b[0m\x1b[31mNew Password: ");
		Reset
		if (strlen(newPassword) <= 0 || strlen(newPassword) > 30) {
			printError2("Error: Password's length must between 1 and %d\n", 30)
			goto inputNewPassword;
		}
		if (strcmp(temp->element.password, newPassword) == 0) {
			printError("Error: The new password must be different from the old one!\n")
			goto inputNewPassword;
		} else {
			updateList2(username, newPassword);
			writeFile();
			printSuccess("Change password success!\n")
		}
	}
}

void logoutFunction() {
	if (!checkIsLoggedIn()) {
		printError("Error: You have not logged in!\n")
		return;
	}
	*isLogin = FALSE;
	printSuccess("Logout success!\n")
}

void visitIpFunction() {
	if (!checkIsLoggedIn()) {
		printError("Error: You have not logged in\n")
		return;
	}
	char *ip = (char *)malloc(sizeof(char) * 30);
	char *domain = (char *)malloc(sizeof(char) * 100);
	strcpy(domain, temp->element.homepage);
	hostname_to_ip(domain, ip);
	printSuccess2("Ip: %s\n", ip);
	free(ip);
	free(domain);
}

void visitDomainFunction() {
	if (!checkIsLoggedIn()) {
		printError("Error: You have not logged in\n")
		return;
	}
	char *ip = (char *)malloc(sizeof(char) * 30);
	char *domain = (char *)malloc(sizeof(char) * 100);
	strcpy(ip, temp->element.homepage);
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
	free(username);
	free(password);
	free(isLogin);
}
