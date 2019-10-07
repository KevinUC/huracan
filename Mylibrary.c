#include "Mylibrary.h"

void myExit()
{
    fprintf(stderr, "Bye...\n");
}

void myCd(char *dest)
{
}

void myPwd()
{
    char buffer[512];
    getcwd(buffer, sizeof(buffer));
    fprintf(stdout, "%s\n", buffer);
}