#include <stdio.h>
#include <string.h>

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
#define printError3(a, b, c) FgRed Bright printf(a, b, c); Reset
#define printSuccess(a) FgGreen Bright printf(a); Reset
#define printWarning(a) FgYellow Bright printf(a); Reset
#define printWarning2(a, b) FgYellow Bright printf(a, b); Reset

#define printTagUsage(a, b) printf("%-15s\t%s\n", a, b);

// type
#define TYPE_IP_ADDRESS "1"
#define TYPE_DOMAIN "2"

// ten file dich
#define BIN_FILE "resolver"
#define RUN_BIN_FILE "./resolver"
// usage
#define HELP_TAG "-h"
#define HELP_TAG2 "--help"
#define SHOW_USAGE "./resolver -h"
#define SHOW_USAGE2 "./resolver --help"

#define VERSION_TAG "-v"
#define VERSION_TAG2 "--version"

#define CONTACT_TAG "--contact"

#define SOURCE_CODE_TAG "--source"

// update date
#define UPDATE_DATE "24/09/2020"