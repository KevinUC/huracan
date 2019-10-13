#include "Mylibrary.h"

void myPwd()
{
    char buffer[512];
    getcwd(buffer, sizeof(buffer));
    fprintf(stdout, "%s\n", buffer);
}