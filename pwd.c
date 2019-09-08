#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/utsname.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <time.h>
#include "globals.h"

void pwd()
{
    char currDir[1024];

    if (Commands[currCommand].argumentsIndex>0)
    {
        dprintf(Commands[currCommand].outputFd, "pwd: Too many arguments\n");
        return;
    }

    if (getcwd(currDir, sizeof(currDir)) != NULL)
        dprintf(Commands[currCommand].outputFd, "%s\n", currDir);
    else
        perror("getcwd() error");
}