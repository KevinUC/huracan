#ifndef MYLIBRARY_GUARD
#define MYLIBRARY_GUARD

#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

void myExit();
void myCd(char *dest);
void myPwd();

#endif