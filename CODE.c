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