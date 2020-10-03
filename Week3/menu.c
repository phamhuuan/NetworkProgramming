#include <stdio_ext.h>
#include <string.h>

#include "helper.h"

void menu(char *menuChoice)
{
	FgBlue
	Bright
	printf("\tUSER MANAGEMENT PROGRAM\n");
	Reset
	FgMagenta;
	printf("----------------------------------------\n");
	printf("1.\t%s\n", "Register");
	printf("2.\t%s\n", "Activate");
	printf("3.\t%s\n", "Sign in");
	printf("4.\t%s\n", "Search");
	printf("5.\t%s\n", "Change password");
	printf("6.\t%s\n", "Sign out");
	printf("7.\t%s\n", "Homepage with domain name");
	printf("8.\t%s\n", "Homepage with IP address");
	FgYellow
	printf("%s", "Your choice (1-8, other to quit): ");
	__fpurge(stdin);
	fgets(menuChoice, sizeof(char) * 32, stdin);
	for (int i = 0; i < strlen(menuChoice) - 1; i++) {
		if(menuChoice[i] < '0' || menuChoice[i] > '9') {
			strcpy(menuChoice, "Error");
			return;
		}
	}
	Reset
}

void menu2(char *menuChoice)
{
	FgMagenta
	printf("1.\t%s\n", "Go to menu");
	printf("2.\t%s\n", "Exit");
	FgYellow
	printf("%s", "Your choice (1-2): ");
	__fpurge(stdin);
	fgets(menuChoice, sizeof(char) * 32, stdin);
	for (int i = 0; i < strlen(menuChoice) - 1; i++) {
		if(menuChoice[i] < '0' || menuChoice[i] > '9') {
			strcpy(menuChoice, "Error");
			return;
		}
	}
	Reset
}