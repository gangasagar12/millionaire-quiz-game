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

typedef struct {
    char name[MAX_NAME_LEN];
    int winnings;
    int correct_answers;
    int lifeline_5050;
    int lifeline_skip;
} ScoreEntry;

void save_score(const char* name, int winnings, int correct_answers, int lifeline_5050, int lifeline_skip);
void show_scoreboard();
int read_questions(const char* filename, Question** questions);
void display_question(int qnum, const Question* q, int money, const int lifelines[], int category);
int handle_answer(const Question* q, char answer, int* money);
int use_lifeline(Question* q, int* lifelines, int* used_5050, int* used_skip);
void clear_input_buffer();
void clear_screen();
char get_answer_with_timer(int seconds, int* timed_out, int* seconds_taken);
void show_category_theme(int category);

int main() {
    srand((unsigned)time(NULL));  //// Seed random number generator
    clear_screen();

    while (1) {
        printf("%s\t\t\t WELCOME  ON THE  QUIZ SHOW %s \n\n", PINK, COLOR_END);
        printf("%s\t\t\t 1. Play game %s \n\n", AQUA);
        printf("%s\t\t\t 2. Show scoreboard %s \n\n", YELLOW);
        printf("%s \t\t\t 3. Exit game %s \n\n", BLUE, COLOR_END);
        printf("%s \t\t\t Enter your choice (1-3): %s \n\n", BLUE, COLOR_END);

        int choice;
        scanf("%d", &choice);
        clear_input_buffer();

        if (choice == 2) {
            show_scoreboard();
            printf("%s\t Press Enter to return to menu...%s \n", GREEN, COLOR_END);
            getchar();
            clear_screen();
            continue;
        }
        if (choice != 1) {
            printf("%s\t\t Exiting the game %s\n", AQUA, COLOR_END);
            return 0;
        }
        
        char player_name[MAX_NAME_LEN];
        printf("%s\t\t\tEnter your name: %s \n", AQUA, COLOR_END);
        fgets(player_name, MAX_NAME_LEN, stdin);
        player_name[strcspn(player_name, "\n")] = 0;  // remove the newline

        printf("%s\t\t\t Choose a category (1-3):%s \n\n", AQUA, COLOR_END);
        printf("%s\t\t\t 1. Sports questions%s \n\n", BLUE, COLOR_END);
        printf("%s\t\t\t 2. History questions%s \n\n", YELLOW, COLOR_END);
        printf("%s\t\t\t 3. Science questions%s \n\n", AQUA, COLOR_END);
