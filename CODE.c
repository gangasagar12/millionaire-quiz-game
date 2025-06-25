#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // is used to include character handling functions like uppercase lowercase
#include <time.h>
#include <windows.h>   // windows api for beep ,sleep, clear screen
#include <conio.h>  // for getch , kbhit

#define MAX_QUES_LEN 300
#define MAX_OPTION_LEN 100
#define MAX_NAME_LEN 50
#define SCOREBOARD_FILE "scoreboard.txt"
#define MAX_SCORES 10

const char* COLOR_END = "\033[0m";
const char* RED = "\033[1;31m";
const char* GREEN = "\033[1;32m";
const char* YELLOW = "\033[1;33m";
const char* BLUE = "\033[1;34m";
const char* PINK = "\033[1;35m";
const char* AQUA = "\033[1;36m";
const char* WHITE = "\033[1;37m";

typedef struct {
    char text[MAX_QUES_LEN];
    char options[4][MAX_OPTION_LEN];
    char correct_option;
    int timeout;
    int prize_money;
} Question;
