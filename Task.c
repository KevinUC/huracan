#include "Task.h"

parseStatus_t readAndParseTasks(Task *tasks, char *command, int *count, bool *bg)
{
    /* prepare to read input from stdin */
    char input[MAX_CML_LENGTH];
    memset(input, 0, sizeof(input));

    fgets(input, sizeof(input), stdin);

    /*
     Echoes command line to stdout if fgets read from a file and not
     the terminal (which is the case with the test script)
     */

    if (!isatty(STDIN_FILENO))
    {
        printf("%s", input);
        fflush(stdout);
    }

    input[strlen(input) - 1] = '\0'; /* remove \n */
    strncpy(command, input, strlen(input));

    if (containOnlySpace(input))
    {
        return PARSE_ONLY_SPACE;
    }

    /* put \n back because we treat it as a special char
     */
    input[strlen(input)] = '\n';

    /* buffer stores program and arguments until flushing */
    char buffer[MAX_ARGS][MAX_CML_LENGTH];

    memset(buffer, 0, sizeof(buffer));

    const int len = strlen(input); /* length of the input string */
    int taskCnt = 0;               /* number of tasks parsed so far */
    int i = 0;                     /* current index */
    int argCnt = 0;                /* number of arguments parsed so far */
    int argCharIdx = 0;
    bool hasIncremented = true; /* indicate if argCnt has been incremented */

    while (i < len)
    {
        char ch = input[i];

        if (!isSpecial(ch))
        {
            /* handle normal char */
            hasIncremented = false;
            buffer[argCnt][argCharIdx] = ch;
            argCharIdx++;
            i++;
            continue;
        }

        if (ch != ' ' || (ch == ' ' && i >= 1 && !isSpecial(input[i - 1])))
        {
            if (!hasIncremented)
            {
                hasIncremented = true;
                argCnt++;
                tasks[taskCnt]._argCnt++; /* update argCnt */
                argCharIdx = 0;

                if (argCnt > MAX_ARGS)
                {
                    return PARSE_ERROR_TOO_MANY_ARGS;
                }
            }

            if (argCnt == 0)
            {
                return PARSE_ERROR_MISSING_CMD;
            }
        }

        if (ch == '&')
        {
            if (!isBgValid(input, i + 1))
            {
                return PARSE_ERROR_MISLOCATED_BG;
            }

            *bg = true;
        }
        else if (ch == '<')
        {
            /* found input redirect */
            tasks[taskCnt]._hasInRedirect = true;

            if (taskCnt > 0)
            {
                return PARSE_ERROR_MISLOCATED_INPUT;
            }

            /* get filename and open */
            char fileName[MAX_CML_LENGTH];
            memset(fileName, 0, sizeof(fileName));

            i++;
            if (!getFileName(input, fileName, &i))
            { /* filename not provided  */
                return PARSE_ERROR_NO_INPUT;
            }

            /* open file and obtain fd */
            int fd = open(fileName, O_RDONLY);

            if (fd < 0)
            { /* open file error */
                return PARSE_ERROR_FAILURE_TO_OPEN_INPUT;
            }

            tasks[taskCnt]._inFileFd = fd; /* overwrite input file fd */
        }
        else if (ch == '>')
        {
            /* found output redirect */
            tasks[taskCnt]._hasOutRedirect = true;

            if (!isOutRedirectValid(input, i + 1))
            {
                return PARSE_ERROR_MISLOCATED_OUTPUT;
            }

            /* get filename and open */
            char fileName[MAX_CML_LENGTH];
            memset(fileName, 0, sizeof(fileName));

            i++;
            if (!getFileName(input, fileName, &i))
            { /* filename not provided  */
                return PARSE_ERROR_NO_OUTPUT;
            }

            /* open file and obtain fd */
            int fd = open(fileName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP);

            if (fd < 0)
            {
                /* open file error */
                return PARSE_ERROR_FAILURE_TO_OPEN_OUTPUT;
            }

            tasks[taskCnt]._outFileFd = fd; /* overwrite output file fd */
        }
        else if (ch == '|' || ch == '\n')
        {
            /* flush buffers to Tasks */

            strncpy(tasks[taskCnt]._program, buffer[0], strlen(buffer[0]));
            tasks[taskCnt]._args = malloc((argCnt + 1) * sizeof(char *));

            for (int j = 0; j < argCnt; j++)
            {
                tasks[taskCnt]._args[j] = malloc(MAX_CML_LENGTH);
                strncpy(tasks[taskCnt]._args[j], buffer[j], strlen(buffer[j]));
            }

            tasks[taskCnt]._args[argCnt] = NULL;
            memset(buffer, 0, sizeof(buffer));

            argCnt = 0;
            taskCnt++;
        }

        i++; /* finally, move on to the next char */
    }

    *count = taskCnt;

    return PARSE_SUCCESS;
}

bool isSpecial(char ch)
{
    return ch == ' ' || ch == '|' || ch == '>' || ch == '<' || ch == '&' || ch == '\n';
}

bool containOnlySpace(char *input)
{
    for (int i = 0; i < strlen(input); i++)
    {
        if (input[i] != ' ')
        {
            return false;
        }
    }
    return true;
}

bool getFileName(char *input, char *fileName, int *i)
{
    /* removing leading spaces */
    while (input[*i] == ' ')
    {
        (*i)++;
    }

    int start = *i;

    if (isSpecial(input[start]))
    {
        /* filename is not provided */
        return false;
    }

    while (!isSpecial(input[*i]))
    {
        (*i)++;
    }

    (*i)--; /* (*i) is the index of the end of filename */

    strncpy(fileName, input + start, (*i) - start + 1);

    return true;
}

bool isBgValid(char *input, int i)
{
    for (; i < strlen(input) - 1; i++)
    {
        if (input[i] != ' ')
        {
            return false;
        }
    }
    return true;
}

bool isOutRedirectValid(char *input, int i)
{
    for (; i < strlen(input) - 1; i++)
    {
        if (input[i] == '|')
        {
            return false;
        }
    }
    return true;
}

void printErrorMessage(parseStatus_t status)
{
    switch (status)
    {
    case PARSE_ERROR_TOO_MANY_ARGS:
        fprintf(stderr, "Error: too many process arguments\n");
        break;
    case PARSE_ERROR_MISSING_CMD:
        fprintf(stderr, "Error: missing command\n");
        break;
    case PARSE_ERROR_MISLOCATED_INPUT:
        fprintf(stderr, "Error: mislocated input redirection\n");
        break;
    case PARSE_ERROR_NO_INPUT:
        fprintf(stderr, "Error: no input file\n");
        break;
    case PARSE_ERROR_MISLOCATED_OUTPUT:
        fprintf(stderr, "Error: mislocated output redirection\n");
        break;
    case PARSE_ERROR_NO_OUTPUT:
        fprintf(stderr, "Error: no output file\n");
        break;
    case PARSE_ERROR_MISLOCATED_BG:
        fprintf(stderr, "Error: mislocated background sign\n");
        break;
    case PARSE_ERROR_FAILURE_TO_OPEN_INPUT:
        fprintf(stderr, "Error: cannot open input file\n");
        break;
    case PARSE_ERROR_FAILURE_TO_OPEN_OUTPUT:
        fprintf(stderr, "Error: cannot open output file\n");
        break;
    case EXECUTE_ERROR_NO_SUCH_DIRECTORY:
        fprintf(stderr, "Error: no such directory\n");
        break;
    case EXECUTE_ERROR_NO_SUCH_COMMAND:
        fprintf(stderr, "Error: command not found\n");
        break;
    case EXECUTE_ERROR_EXIT_WITH_BG:
        fprintf(stderr, "Error: active jobs still running\n");
        break;
    default:
        break;
    }
}
