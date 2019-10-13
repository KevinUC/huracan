#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#include "Task.h"
#include "Helpers.h"
#include "List.h"

int main(int argc, char *argv[])
{
    Task tasks[MAX_NUM_TASKS]; /* storing task objects */
    List bgList;
    /* initialize bgList */
    memset(&bgList, 0, sizeof(List));
    Node *head = malloc(sizeof(Node));
    head->_next = NULL;
    bgList._head = head;

    while (1)
    {
        int taskCnt = 0;
        bool bg = false;
        char command[MAX_CML_LENGTH]; /* user command */

        memset(tasks, 0, sizeof(tasks));
        memset(command, 0, sizeof(command));

        /* check if bg task have been completed and print out the exit status message */
        if (bgList._size > 0)
        {
            processList(&bgList);
        }

        displayPrompt();

        parseStatus_t status = readAndParseTasks(tasks, command, &taskCnt, &bg);

        if (status == PARSE_SUCCESS)
        {
            //printTasks(tasks, taskCnt);
            executeCommands(tasks, command, taskCnt, bg, &bgList);
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

    /* clean up Node */
    if (bgList._head != NULL)
    {
        free(bgList._head);
        bgList._head = NULL;
    }

    return EXIT_SUCCESS;
}