#include <stdio.h>
#include "helper.h"

void menu(int *menuChoice)
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
	FgYellow
	printf("%s", "Your choice (1-6, other to quit): ");
	scanf("%d", menuChoice);
	Reset
}

void menu2(int *menuChoice)
{
	FgMagenta
	printf("1.\t%s\n", "Go to menu");
	printf("2.\t%s\n", "Exit");
	FgYellow
	printf("%s", "Your choice (1-2): ");
	scanf("%d", menuChoice);
	Reset
}