#include "Helpers.h"

/* display shell prompt to the user*/

void displayPrompt()
{
    const char *shellPrompt = "sshell$ ";
    fputs(shellPrompt, stdout);
}

void executeCommands(Task *tasks, char *command, int taskCnt, bool bg)
{
    pid_t pidNum;
    pid_t taskToPid[MAX_CML_LENGTH];
    int taskToExitStatus[MAX_CML_LENGTH];
    memset(taskToPid, -1, sizeof(taskToPid));
    memset(taskToExitStatus, 0, sizeof(taskToExitStatus));

    int fd[MAX_NUM_TASKS][2];
    int stdoutFdCpy = dup(STDOUT_FILENO); /* save fd to stdout */
    int stdinFdCpy = dup(STDIN_FILENO);   /* save fd to stdin */
    bool hasExit = false;

    for (int i = 0; i < taskCnt; i++)
    {
        /* The following code block was partially based on lecture slide 01.syscalls, page 36 */

        if (i > 0)
        { /* if there are previous commands */
            dup2(fd[i - 1][0], STDIN_FILENO);
            close(fd[i - 1][0]); /* close read */
        }

        if (i < taskCnt - 1)
        {                /* if not the last command  */
            pipe(fd[i]); /* create pipe for the inter-coommunication between cur and next commands */
            dup2(fd[i][1], STDOUT_FILENO);
            close(fd[i][1]); /* close write */
        }

        if (i == taskCnt - 1)
        {                                     /* last command */
            dup2(stdoutFdCpy, STDOUT_FILENO); /* recover stdout from the copy */
        }

        pidNum = fork(); /* fork() clones a new process */

        if (pidNum != 0)
        { /* Parent */
            taskToPid[i] = pidNum;
            if (strcmp(tasks[i]._program, "exit") == 0)
            {
                hasExit = true;
            }
            else if (strcmp(tasks[i]._program, "cd") == 0)
            {
                if (chdir(tasks[i]._args[1]) != 0)
                {
                    printErrorMessage(EXECUTE_ERROR_NO_SUCH_DIRECTORY);
                    taskToExitStatus[i] = 1;
                }
            }
        }
        else
        { /* Child */
            executeSingleCommand(tasks[i]);
        }
    }

    dup2(stdinFdCpy, STDIN_FILENO); /* recover stdin from the copy */

    /* wait for processes to finish */
    for (int i = 0; i < taskCnt; i++)
    {
        int status;
        int completedPidNum = wait(&status);
        int taskNum = findTaskNum(taskToPid, completedPidNum, taskCnt);

        taskToExitStatus[taskNum] = WEXITSTATUS(status);
    }

    if (hasExit) /* kill the parent process */
    {
        exit(0);
    }

    /* all tasks have completed, print message to sterr */
    fprintf(stderr, "+ completed \'%s\' ", command);

    for (int i = 0; i < taskCnt; i++)
    {
        fprintf(stderr, "[%d]", taskToExitStatus[i]);
    }

    fprintf(stderr, "\n");
}

void executeSingleCommand(Task task)
{
    if (task._hasInRedirect)
    {
        dup2(task._inFileFd, STDIN_FILENO); /* redirect stdin to input file */
        close(task._inFileFd);
    }

    if (task._hasOutRedirect)
    {
        dup2(task._outFileFd, STDOUT_FILENO); /* redirect stdin to input file */
        close(task._outFileFd);
    }

    /* built in command */
    if (strcmp(task._program, "exit") == 0)
    {
        myExit();
    }
    else if (strcmp(task._program, "cd") == 0)
    {
        myCd("");
    }
    else if (strcmp(task._program, "pwd") == 0)
    {
        myPwd();
    }
    else
    {
        execvp(task._program, (char *const *)task._args);
        if (errno == ENOENT)
        {
            printErrorMessage(EXECUTE_ERROR_NO_SUCH_COMMAND);
            exit(1);
        }
        exit(errno); /* if execvp fails, catch the error */
    }

    exit(0);
}

int findTaskNum(pid_t *taskToPid, pid_t pidNum, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (pidNum == taskToPid[i])
        {
            return i;
        }
    }
    //printf("findTaskNum fails ... \n\n");
    return -1;
}