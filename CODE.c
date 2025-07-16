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
int category;
        scanf("%d", &category);
        clear_input_buffer();
        if (category < 1 || category > 3) {
            printf("%s\t \t Invalid category. Returning to menu...%s\n", RED, COLOR_END);
            continue;
        }

        const char* files[] = {"Sports.txt", "History.txt", "Science.txt"};
        Question* questions = NULL;
        int question_count = read_questions(files[category - 1], &questions);
        if (question_count <= 0) {
            printf("%sFailed to load questions. Returning to menu.%s\n", RED, COLOR_END);
            continue;
        }
        
        int total_money = 0;
        int lifelines[] = {1, 1}; // 50-50, Skip
        int correct_answers = 0;
        int used_5050 = 0;
        int used_skip = 0;
        int i;
        for (i = 0; i < question_count; i++) {
            clear_screen();
            show_category_theme(category);
            display_question(i + 1, &questions[i], total_money, lifelines, category);
            char answer = 0;
            int answered = 0, timed_out = 0, seconds_taken = 0;

            while (!answered) {   //  wait for answer
                answer = get_answer_with_timer(questions[i].timeout, &timed_out, &seconds_taken);   // get answer with timer

                if (timed_out) {
                    printf("%sTime's up! The correct answer was %c.%s\n", YELLOW, questions[i].correct_option, COLOR_END);
                    printf("%sFinal winnings: Rs %d%s\n", RED, total_money, COLOR_END);
                    Beep(600, 350);
                    save_score(player_name, total_money, correct_answers, used_5050, used_skip);
                    free(questions);
                    printf("%sPress Enter to exit...%s", GREEN, COLOR_END);
                    getchar();
                    return 0;
                }
                
                if (answer == 'L') {
                    int result = use_lifeline(&questions[i], lifelines, &used_5050, &used_skip);
                    if (result == 1) {
                        clear_screen();
                        show_category_theme(category);
                        display_question(i + 1, &questions[i], total_money, lifelines, category);
                        Beep(800, 120);
                    } else if (result == 0) {  // skip lifeline , move to next question
                        answered = 1;
                        break;
                    }
                    continue;
                }
                if (answer >= 'A' && answer <= 'D') {
                    int correct = handle_answer(&questions[i], answer, &total_money);
                    if (correct) {
                        correct_answers++;
                        Beep(1200, 170);
                    } else {
                        Beep(350, 250);
                        printf("%sGame Over! Final winnings: Rs %d%s\n", RED, total_money, COLOR_END);
                        save_score(player_name, total_money, correct_answers, used_5050, used_skip);
                        free(questions);
                        printf("%sPress Enter to exit...%s", GREEN, COLOR_END);
                        getchar();
                        return 0;
                    }
                    answered = 1;
                } else {
                    printf("%sInvalid choice. Please enter A-D or L.%s\n", RED, COLOR_END);
                }
            }
            printf("%sPress Enter to continue...%s", GREEN, COLOR_END);
            clear_input_buffer();
        }
printf("%sCongratulations, %s! You answered all questions!\nTotal winnings: Rs %d%s\n", BLUE, player_name, total_money, COLOR_END);
        save_score(player_name, total_money, correct_answers, used_5050, used_skip);  // save final score
        free(questions);
        printf("%sPress Enter to exit...%s", GREEN, COLOR_END);
        getchar();
        return 0;
    }
    return 0;
}
void save_score(const char* name, int winnings, int correct_answers, int lifeline_5050, int lifeline_skip) {
    // Read all entries, replace if name matches, otherwise append
    ScoreEntry entries[MAX_SCORES];
    int total = 0, found = 0;
    FILE* file = fopen(SCOREBOARD_FILE, "r");
    if (file) {
        while (fscanf(file, "%49[^,],%d,%d,%d,%d\n", entries[total].name, &entries[total].winnings, &entries[total].correct_answers, &entries[total].lifeline_5050, &entries[total].lifeline_skip) == 5 && total < MAX_SCORES) {
            if (strcmp(entries[total].name, name) == 0) {
                entries[total].winnings = winnings;
                entries[total].correct_answers = correct_answers;
                entries[total].lifeline_5050 = lifeline_5050;
                entries[total].lifeline_skip = lifeline_skip;
                found = 1;
            }
            total++;
        }
        fclose(file);
        }
    if (!found && total < MAX_SCORES) {  // new entry if not found
        strncpy(entries[total].name, name, MAX_NAME_LEN);
        entries[total].winnings = winnings;
        entries[total].correct_answers = correct_answers;
        entries[total].lifeline_5050 = lifeline_5050;
        entries[total].lifeline_skip = lifeline_skip;
        total++;
    }
    int i;
    file = fopen(SCOREBOARD_FILE, "w");
    for (i = 0; i < total; i++) {
        fprintf(file, "%s,%d,%d,%d,%d\n", entries[i].name, entries[i].winnings, entries[i].correct_answers, entries[i].lifeline_5050, entries[i].lifeline_skip);
    }
    fclose(file);
}

void show_scoreboard() {
    ScoreEntry scores[MAX_SCORES];
    int total = 0;
    FILE* file = fopen(SCOREBOARD_FILE, "r");
    if (!file) {
        printf("%sNo scores yet!%s\n", YELLOW, COLOR_END);
        return;
    }
 while (fscanf(file, "%49[^,],%d,%d,%d,%d\n", scores[total].name, &scores[total].winnings, &scores[total].correct_answers, &scores[total].lifeline_5050, &scores[total].lifeline_skip) == 5 && total < MAX_SCORES) {
        total++;
    }
    fclose(file);

    // Sort by winnings (descending)
    int i,j;
    for (i = 0; i < total - 1; i++) {
        for (j = i + 1; j < total; j++) {
            if (scores[j].winnings > scores[i].winnings) {
                ScoreEntry temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }

    printf("\n========== PLAYER SCOREBOARD ==========\n");
    printf("%-20s %-10s %-17s %-15s %-15s\n", "Name", "Winnings", "Correct Answers", "50-50 Used", "Skip Used");
    printf("===================================================================================\n");
    for (i = 0; i < total && i < 10; i++) {
        printf("%-20s Rs %-8d %-17d %-15s %-15s\n",
            scores[i].name,
            scores[i].winnings,
            scores[i].correct_answers,
            scores[i].lifeline_5050 > 0 ? "Yes" : "No",
            scores[i].lifeline_skip > 0 ? "Yes" : "No"
        );
    }
    if (total == 0) {
        printf("%-20s %-10s\n", "No scores", "yet!");
    }
    printf("===================================================================================\n\n");
}

int read_questions(const char* filename, Question** questions) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("%sCould not load file.%s\n", RED, COLOR_END);
        return 0;
    }
    char buffer[MAX_QUES_LEN];
    int lines = 0;
    while (fgets(buffer, sizeof(buffer), file)) 
        lines++;
    int count = lines / 8;

    *questions = malloc(count * sizeof(Question));
    if (*questions == NULL) {
        printf("%sMemory allocation failed!%s\n", RED, COLOR_END);
        return 0;
    }

    int i,j;
    rewind(file);
    for (i = 0; i < count; i++) {
        fgets((*questions)[i].text, MAX_QUES_LEN, file);
        (*questions)[i].text[strcspn((*questions)[i].text, "\n")] = 0;  // remove newline 
        for (j = 0; j < 4; j++) {
            fgets((*questions)[i].options[j], MAX_OPTION_LEN, file);
            (*questions)[i].options[j][strcspn((*questions)[i].options[j], "\n")] = 0;
        }
        char correct[10], timeout[10], prize[20];
        fgets(correct, sizeof(correct), file);
        (*questions)[i].correct_option = toupper(correct[0]);
        fgets(timeout, sizeof(timeout), file);
        (*questions)[i].timeout = atoi(timeout);
        fgets(prize, sizeof(prize), file);
        (*questions)[i].prize_money = atoi(prize);
    }
        fclose(file);
    return count;  // number of questions loaded
}

void display_question(int qnum, const Question* q, int money, const int lifelines[], int category) {
    int i;
    const char* theme_color = AQUA;
    if (category == 1) theme_color = BLUE;
    else if (category == 2) theme_color = YELLOW;
    else if (category == 3) theme_color = GREEN;
printf("%s------------- Question %d -------------\n", theme_color, qnum);
    printf("%sPrize: Rs %d\n", GREEN, q->prize_money);
    printf("%sCurrent winnings: Rs %d\n", BLUE, money);
    printf("%sLifelines: 50-50 (%s) | Skip (%s)\n", PINK,
           lifelines[0] ? "available" : "used",
           lifelines[1] ? "available" : "used");
    printf("%sTimer: %d seconds\n", AQUA, q->timeout);
    printf("\n%s%s\n", theme_color, q->text);
    for (i = 0; i < 4; i++) {
        if (q->options[i][0] != '\0') {
            printf("%c) %s\n", 'A' + i, q->options[i]);
        }
    }
}
int handle_answer(const Question* q, char answer, int* money) {
    if (answer == q->correct_option) {
        *money += q->prize_money;
        return 1;
    } else {
        printf("%sWrong! Correct answer was %c%s\n", RED, q->correct_option, COLOR_END);
        return 0;
    }
}
int use_lifeline(Question* q, int* lifelines, int* used_5050, int* used_skip) {
    printf("%sAvailable lifelines:%s\n", BLUE, COLOR_END);
    if (lifelines[0]) printf("1) 50-50\n");
    if (lifelines[1]) printf("2) Skip Question\n");
    printf("0) Cancel\nChoose: ");

    char choice;
    scanf(" %c", &choice);
    clear_input_buffer();

    switch (choice) {
        case '1':
            if (!lifelines[0]) {
                printf("%s50-50 already used!%s\n", RED, COLOR_END);
                return 2;
            }
              lifelines[0] = 0;
            (*used_5050)++;
            {
                int removed = 0;
                while (removed < 2) {
                    int idx = rand() % 4;
                    if (('A' + idx) != q->correct_option && q->options[idx][0] != '\0') {
                        q->options[idx][0] = '\0';
                        removed++;
                    }
                }
            }
            printf("%sTwo incorrect options removed!%s\n", GREEN, COLOR_END);
            return 1;
            case '2':
            if (!lifelines[1]) {
                printf("%sSkip already used!%s\n", RED, COLOR_END);
                return 2;
            }
            lifelines[1] = 0;
            (*used_skip)++;
            printf("%sQuestion skipped!%s\n", YELLOW, COLOR_END);
            return 0;
        default:
            return 2;
    }
}

