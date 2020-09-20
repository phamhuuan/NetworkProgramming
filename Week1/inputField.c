#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>

#include "helper.h"

void inputString(char *holder, int size, char *result)
{
	FgRed
	printf("%s", holder);
	__fpurge(stdin);
	fgets(result, size, stdin);
	for (int i = 0; i < strlen(result); i++)
	{
		if (result[i] == '\n')
		{
			result[i] = '\0';
		}
	}
	Reset
}