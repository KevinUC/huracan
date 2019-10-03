#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define MAXCML 512

int main(int argc, char *argv[])
{

    int retval;
    const char *shellPrompt = "sshell$ ";

    while (1)
    {
        char cmd[MAXCML];

        fputs(shellPrompt, stdout);

        fgets(cmd, sizeof cmd, stdin);

        cmd[strlen(cmd) - 1] = '\0';

        char **argvs = (char *[]){cmd, NULL};

        pid_t pid = fork();

        if (pid == 0)
        { /* enter child proecess */
            execvp(cmd, argvs);
        }
        else
        { /* enter parent proecess */
            wait(&retval);
            //char *cmdFull = "/bin/date -u";
            //fprintf(stderr, "Return status value for '%s': %d\n", cmdFull, WEXITSTATUS(retval));
        }
    }

    return EXIT_SUCCESS;
}