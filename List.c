#include "List.h"
#include "Helpers.h"

void addNode(List *list, Node *node)
{
    Node *cur = list->_head;

    while (cur->_next != NULL)
    {
        cur = cur->_next;
    }

    cur->_next = node;
    node->_next = NULL;

    list->_size++;
}

void processList(List *list)
{
    Node *prev = list->_head;
    Node *cur = prev->_next;

    bool hasCompleted = true;

    while (cur != NULL)
    {
        /*check if current bg command has completed */
        int taskCnt = cur->_taskCnt;
        char *command = cur->_command;
        pid_t *taskToPid = cur->_taskToPid;
        int *taskToExitStatus = cur->_taskToExitStatus;
        int status;

        for (int i = 0; i < taskCnt; i++)
        {
            int ret = waitpid(taskToPid[i], &status, WNOHANG);

            if (ret > 0)
            {
                /* child has completed */
                taskToExitStatus[i] = WEXITSTATUS(status);
            }
            else if (ret == 0)
            {
                hasCompleted = false; /* child is still running */
            }
        }

        if (!hasCompleted)
        {
            /* cannot print message for current command , need to wait for all
               of its children to complete */
            prev = cur;
            cur = cur->_next;
            continue;
        }

        /* print message to stderr */
        printCommandExitStatus(command, taskCnt, taskToExitStatus);

        /* remove cur node from list, decrement list size */
        prev->_next = cur->_next;
        Node *tmp = cur;
        cur = cur->_next;
        free(tmp);
        list->_size--;
    }
}