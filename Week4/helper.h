#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// input from shell
void inputString(char *holder, int size, char *result);

// menu
void menu();
void menu2();

// danh sach lien ket
typedef struct
{
	char username[100];
	char password[100];
	char status[2];
	char homepage[100];
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
#define printSuccess2(a, b) FgGreen Bright printf(a, b); Reset
#define printWarning(a) FgYellow Bright printf(a); Reset
#define printWarning2(a, b) FgYellow Bright printf(a, b); Reset

// status
#define BLOCKED 0
#define ACTIVE 1
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

#define BUFF_SIZE 4096

int initSocket(int *sockfd);
int hostname_to_ip(char *  , char *);
int ip_to_hostname(char *, char *);
int startServer(int *sockfd, struct sockaddr_in *servaddr, const unsigned int SERVER_PORT);
int startClient(char *ip, struct sockaddr_in *servaddr, const unsigned int SERVER_PORT);
int checkValidPort(const unsigned int SERVER_PORT);
int checkValidIp(char *ip);
int checkValidDomain(char *domain);
int listenMessage(int *sockfd, char *buff, struct sockaddr_in *addr);
int sendMessage(int *sockfd, char *buff, struct sockaddr_in *addr);

#define FIND_ACCOUNT 0
#define FIND_ACCOUNT_NOT_FOUND 1
#define FIND_ACCOUNT_FOUND 2

#define REGISTER 3
#define REGISTER_SUCCESS 4

#define BLOCK_ACCOUNT 5
#define BLOCK_ACCOUNT_SUCCESS 6

#define ACTIVE_ACCOUNT 7
#define ACTIVE_ACCOUNT_SUCCESS 8

#define CHANGE_PASSWORD 9
#define CHANGE_PASSWORD_SUCCESS 10
