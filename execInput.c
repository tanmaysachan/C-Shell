#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#include "globals.h"
#include "ls.h"

int count = 1;
char proc_stack[1024][1024];

void procExit()
{
    int status;

    for (int i = 1; i < pidTop; i++)
    {
        if (waitpid(pidStack[i], &status, WNOHANG) > 0)
        {
            if (WIFEXITED(status) > 0)
                printf("%s with pid %d exited normally\n", processStack[i], pidStack[i]);
            else if (WIFSIGNALED(status))
                printf("%s with pid %d exited with signal\n", processStack[i], pidStack[i]);
            else
                printf("%s with pid %d exited abnormally\n", processStack[i], pidStack[i]);
        }
    }
}

int execInput()
{
    signal(SIGCHLD, procExit);
    int status;
    pid_t pid = fork();
    if (pid == 0)
    {
        // printf("%s\n%s\n%s\n", command, flags, arguments);
        char *buf[1024];
        buf[0] = (char *)malloc(1024);
        strcpy(buf[0], Commands[currCommand].command);

        int bufSize = 1;

        char *temp;
        temp = (char *)malloc(1024);

        for (int i = 0; i < Commands[currCommand].flagsIndex; i++)
        {
            buf[bufSize] = (char *)malloc(1024);
            strcpy(buf[bufSize++], Commands[currCommand].flags[i]);
        }

        for (int i = 0; i < Commands[currCommand].argumentsIndex; i++)
        {
            buf[bufSize] = (char *)malloc(1024);
            strcpy(buf[bufSize++], Commands[currCommand].arguments[i]);
        }

        dup2(Commands[currCommand].inputFd, 0);
        buf[bufSize] = NULL;
        dup2(Commands[currCommand].outputFd, 1);
        if (execvp(buf[0], buf) < 0)
        {
            printf("*** ERROR: exec failed\n");
            exit(1);
        }
        exit(0);
    }

    else if (pid > 0)
    {
        pidStack[pidTop] = pid;
        strcpy(processStack[pidTop++], Commands[currCommand].command);

        if (Commands[currCommand].backgroundFlag)
            printf("[%d] %d\n", count++, pid);
        else
            waitpid(pid, &status, 0);
    }
    else
    {
        printf("fork error\n");
    }

    return status;
}
