/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
using namespace std;//Dror

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "signals.h"
#define MAX_LINE_SIZE 80
#define MAXARGS 20

char* L_Fg_Cmd;
char lineSize[MAX_LINE_SIZE]; 

//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
	//Initialize the shell
	InitSmash();


    char cmdString[MAX_LINE_SIZE];

    while (1){
    	//smash is waiting for a new cmd
	 	printf("smash > ");
	 	//get full line from user
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);    	
		cmdString[strlen(lineSize)-1]='\0';
		// perform a complicated Command
		if(!ExeComp(lineSize)) continue; 

		else ExeCmd(lineSize, cmdString);

		
		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
    return 0;
}

