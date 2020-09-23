#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "helper.h"

FILE *f;
char *username, *password;
int *isLogin;

// ham dang ki tai khoan
void registerFunction();

// ham active tai khoan
void activeFunction();

// ham login
void loginFunction();

// ham tim kiem tai khoan
void searchFunction();

// ham thay doi mat khau
void changePasswordFunction();

// ham dang xuat
void logoutFunction();

// ham thoat app
void exitFunction();

// ham ghi du lieu vao file
void writeFile();

// ham thay doi trang thai cua mot user trong list
void updateList(char *username, int status);

// ham thay doi mat khau cua mot user trong list
void updateList2(char *username, char *password);

int main() {
	f = fopen(DATA_FILE, "r");
	if (f == NULL) {
		printError("Error: Cannot open file\n")
		return 0;
	} else {
		elementtype *user = (elementtype *)malloc(sizeof(elementtype));
		while (fscanf(f, "%s%s%d", user->username, user->password, &user->status) != EOF) {
			insertBefore(*user);
		}
		fclose(f);
		free(user);
	}
	int *menuChoice;
	menuChoice = (int *)malloc(sizeof(int));
	if (menuChoice == NULL) {
		printError("Error: Memory allocation failed!\n")
		return 0;
	}
	username = (char *)malloc(sizeof(char) * 30);
	password = (char *)malloc(sizeof(char) * 30);
	isLogin = (int *)malloc(sizeof(int));
	*isLogin = FALSE;
openMenu:
	menu(menuChoice);
	switch (*menuChoice) {
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
	default:
		exitFunction();
		return 0;
	}
	int *menuChoice2;
	menuChoice2 = (int *)malloc(sizeof(int));
	if (menuChoice2 == NULL) {
		printError("Error: Memory allocation failed!\n")
		return 0;
	}
openMenu2:
	menu2(menuChoice2);
	switch (*menuChoice2) {
	case 1:
		free(menuChoice2);
		goto openMenu;
		break;
	case 2:
		free(menuChoice2);
		free(menuChoice);
		exitFunction();
		return 0;
	default:
		goto openMenu2;
		break;
	}
}

void writeFile() {
	f = fopen(DATA_FILE, "w+");
	if (f == NULL) {
		printError("Error: Cannot open file\n")
	} else {
		root = reverseList(root);
		traversingList {
			fprintf(f, "%s %s %d\n", p->element.username, p->element.password, p->element.status);
		}
	}
	fclose(f);
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
	password = getpass("\x1b[0m\x1b[31mPassword: ");
	Reset
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
		printf("activationCode %s\n", activationCode);
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

void exitFunction() {
	FgCyan
	printf("Thanks for using my application!\n");
	printf("Visit me at github: https://github.com/phamhuuan/NetworkProgramming\n");
	Reset
	free(username);
	free(password);
	free(isLogin);
}
