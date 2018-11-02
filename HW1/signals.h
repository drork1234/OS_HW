#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


//child handler for SIGINT (SIGTSTP and SIGCONT are handled by default)
void child_handler(int signo);
void parent_handler(int signo);
void SIGCHLD_handler(int signo);

#endif

