#ifndef LIST_GUARD
#define LIST_GUARD

#include "Task.h"

typedef struct Node
{
    int _taskCnt; /* number of tasks in the command */
    char _command[MAX_CML_LENGTH];
    pid_t _taskToPid[MAX_CML_LENGTH];      /* map task index to pid */
    int _taskToExitStatus[MAX_CML_LENGTH]; /* map task index to exit status */
    struct Node *_next;                    /* ptr to next Node */
} Node;

typedef struct
{
    Node *_head;
    int _size;
} List;

void addNode(List *list, Node *node);
void processList(List *list);

#endif
