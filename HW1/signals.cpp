// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
 Synopsis: handle the Control-C */
#include "signals.h"
#include <iostream>

using namespace std;

//********************Child Handlers
//********************************************
// function name: child_handler
// Description: a signal handler for SIGINT - exiting if signaled SIGINT
// Parameters: the signal value (signo)
// Returns: void
//**************************************************************************************
void child_handler(int signo) {
	//exiting for SIGINT.
	//normal behavior (SIG_DFL) for SIGCONT and SIGTSTP
	if (signo == SIGINT) {
		_exit(EXIT_SUCCESS);
	}

}
