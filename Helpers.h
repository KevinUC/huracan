#ifndef HELPERS_GUARD
#define HELPERS_GUARD

#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "Task.h"
#include "Mylibrary.h"
#include "List.h"

void displayPrompt();
void executeCommands(Task *tasks, char *command, int taskCnt, bool bg, List *bgList);
void executeSingleCommand(Task task);
int findTaskNum(pid_t *taskToPid, pid_t pidNum, int len);
void printCommandExitStatus(char *command, int taskCnt, int *taskToExitStatus);

#endif