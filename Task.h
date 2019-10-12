#ifndef TASK_GUARD
#define TASK_GUARD

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_NUM_TASKS 256
#define MAX_CML_LENGTH 512
#define MAX_ARGS 16

typedef enum
{
    PARSE_SUCCESS,
    PARSE_ERROR_TOO_MANY_ARGS,
    PARSE_ERROR_MISSING_CMD,
    PARSE_ERROR_MISLOCATED_INPUT,
    PARSE_ERROR_NO_INPUT,
    PARSE_ERROR_MISLOCATED_OUTPUT,
    PARSE_ERROR_NO_OUTPUT,
    PARSE_ERROR_MISLOCATED_BG,
    PARSE_ERROR_FAILURE_TO_OPEN_INPUT,
    PARSE_ERROR_FAILURE_TO_OPEN_OUTPUT,
    EXECUTE_ERROR_NO_SUCH_DIRECTORY,
    EXECUTE_ERROR_NO_SUCH_COMMAND,
    EXECUTE_ERROR_EXIT_WITH_BG,
    PARSE_ONLY_SPACE
} parseStatus_t;

typedef struct
{
    char _program[MAX_CML_LENGTH]; /* program to be executed, e.g. ls, cd */
    char **_args;                  /* arguments to the program */
    bool _hasInRedirect;           /* mark if the task contains input redirect */
    bool _hasOutRedirect;          /* mark if the task contains output redirect */
    int _inFileFd;
    int _outFileFd;
    int _argCnt;
} Task;

parseStatus_t readAndParseTasks(Task *tasks, char *command, int *count, bool *bg);
bool isSpecial(char ch);
bool containOnlySpace(char *input);
bool getFileName(char *input, char *fileName, int *i);
bool isBgValid(char *input, int i);
bool isOutRedirectValid(char *input, int i);
void printErrorMessage(parseStatus_t status);

/***********HELPER FUNCTIONS**********/

void printTasks(Task *tasks, int cnt);
void printTask(Task task);

#endif