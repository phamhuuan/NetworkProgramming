// input from shell
extern void inputString(char *holder, int size, char *result);

// menu
extern void menu();
extern void menu2();

// danh sach lien ket
typedef struct
{
	char username[30];
	char password[30];
	int status;
} user;

typedef user elementtype;

typedef struct node node;
typedef struct node
{
	elementtype element;
	node *next;
} node;

node *root, *cur, *prev, *temp;

extern node *makeNewNode(elementtype element);
extern void displayNode(node *p);
extern void insertAtHead(elementtype element);
extern void insertAfter(elementtype element);
extern void insertBefore(elementtype element);
extern void deleteFirst();
extern void deleteNode();
#define traversingList for (node *p = root; p != NULL; p = p->next)
extern node *find(char *username);
extern node *reverseList(node *root);
extern void freeList();

// color
#define Reset printf("\x1b[0m");
#define Bright printf("\x1b[1m");
#define Dim printf("\x1b[2m");
#define Underscore printf("\x1b[4m");
#define Blink printf("\x1b[5m");
#define Reverse printf("\x1b[7m");
#define Hidden printf("\x1b[8m");
#define FgBlack printf("\x1b[30m");
#define FgRed printf("\x1b[31m");
#define FgGreen printf("\x1b[32m");
#define FgYellow printf("\x1b[33m");
#define FgBlue printf("\x1b[34m");
#define FgMagenta printf("\x1b[35m");
#define FgCyan printf("\x1b[36m");
#define FgWhite printf("\x1b[37m");
#define BgBlack printf("\x1b[40m");
#define BgRed printf("\x1b[41m");
#define BgGreen printf("\x1b[42m");
#define BgYellow printf("\x1b[43m");
#define BgBlue printf("\x1b[44m");
#define BgMagenta printf("\x1b[45m");
#define BgCyan printf("\x1b[46m");
#define BgWhite printf("\x1b[47m");

// print
#define printError(a) FgRed Bright printf(a); Reset
#define printError2(a, b) FgRed Bright printf(a, b); Reset
#define printSuccess(a) FgGreen Bright printf(a); Reset
#define printWarning(a) FgYellow Bright printf(a); Reset
#define printWarning2(a, b) FgYellow Bright printf(a, b); Reset

// status
#define ACTIVE 0
#define BLOCKED 1
#define IDLE 2

// activation code
#define ACTIVATION_CODE "LTM121216"

// data file
#define DATA_FILE "account.txt"

// max times to enter password/code
#define MAX_TIMES 3

// boolean
#define TRUE 1
#define FALSE 0