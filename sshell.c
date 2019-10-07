#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#include "Task.h"
#include "Helpers.h"

int main(int argc, char *argv[])
{
    Task tasks[MAX_NUM_TASKS]; /* storing task objects */

    while (1)
    {
        int taskCnt = 0;
        bool bg = false;
        char command[MAX_CML_LENGTH]; /* user command */

        memset(tasks, 0, sizeof(tasks));
        memset(command, 0, sizeof(command));

        displayPrompt();

        parseStatus_t status = readAndParseTasks(tasks, command, &taskCnt, &bg);

        if (status == PARSE_SUCCESS)
        {
            //printTasks(tasks, taskCnt);
            executeCommands(tasks, command, taskCnt, bg);
        }
        else
        {
            printErrorMessage(status);
        }

        for (int i = 0; i < taskCnt; i++)
        {
            for (int j = 0; j < tasks[i]._argCnt; j++)
            {
                free(tasks[i]._args[j]);
            }
            free(tasks[i]._args);
        }
    }
    return EXIT_SUCCESS;
}