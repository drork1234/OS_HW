//commands.c
//********************************************
#include "commands.h"
#include <errno.h>
#include <cstdlib>
#include <signal.h>
#include "shell.h"
#include "signals.h"
#define DEBUG 1
#undef DEBUG
using namespace std;

//***********************************helper method**********************
//********************************************
// function name: __reconstruct_from_args
// Description: takes a NULL terminated string array and concatenates all the strings to a single string
//				(needed for command line reconstruction)
// Parameters: A NULL terminated C string array
// Returns: std::string of the reconstructed line from the arguments
//**************************************************************************************
string __reconstruct_from_args(char* args[]){
	string stream;
	for(unsigned i = 0; i < MAX_ARG; i++){
		if(NULL == args[i]) break;
			stream += string(args[i]) + " ";
	}
	stream.resize(stream.size() - 1); //delete the last whitespace
	return stream;
}

//***********************************child handler registering**********
//********************************************
// function name: __register_child_handlers
// Description: registers a the child handler method for SIGINT, a default behavior for SIGTSTP and ignoring SIGCHLD
// Parameters: None
// Returns: void
//**************************************************************************************
void __register_child_handlers(){
	struct sigaction sa;
	sa.sa_handler = child_handler;
	sa.sa_flags = 0;
	sigfillset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) == -1)
		cout << endl << "can't catch SIGINT" << endl;
	if (signal(SIGTSTP, SIG_DFL) == SIG_ERR)
		cout << endl << "can't reassign SIGTSTP in child process!" << endl;
	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR)
		cout << endl << "can't reassign SIGCHLD in child process!" << endl;
}


//*************************API******************************
//********************************************
// function name: ExeCmd
// Description: interprets and executes built-in commands/external commands
// Parameters: command string (not parsed)
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(char* lineSize, char* cmdString)
{
	//if the line is empty, return
	if(string(cmdString).empty()) return 0;
	Shell& shl = Shell::SingInst();
	//save the line to the history log
	shl.AddHistory(cmdString);

	char* cmd; 
	char* args[MAX_ARG];
	const char* delimiters = " \t\n";
	int i = 0, num_arg = 0;
	bool illegal_cmd = false; // illegal command
	//Extract the command
    cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		//when giving NULL as parameter strtok return the next word till the end of string
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++; 
 
	}
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	//Dror
	if (!strcmp(cmd, "cd") ) 
	{
		illegal_cmd = (num_arg != 1);
		if (!illegal_cmd)
			shl.Chdir(args[1]);
	} 
	
	/*************************************************/
	//Dror
	else if (!strcmp(cmd, "pwd")) 
	{
		illegal_cmd = (num_arg != 0);
		if (!illegal_cmd)
			printf("%s\n", shl.PWD().c_str());

	}
	
	/*************************************************/
	//Dror
	else if (!strcmp(cmd, "mkdir"))
	{
		illegal_cmd = (num_arg != 1);
		if(!illegal_cmd)
			shl.Mkdir(args[1]);
	}
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")) 
	{
		illegal_cmd = (num_arg != 0);
		if (!illegal_cmd)
			shl.PrintJobs();
	}
	//dror
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		illegal_cmd = (num_arg != 0);
		if (!illegal_cmd)
			printf("smash pid is %d\n",Shell::SingInst().PID());//check what happens if getpid() failed
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		illegal_cmd = (num_arg > 1);
		if(!illegal_cmd)
			shl.Foreground((num_arg == 0) ? -1 : atoi(args[1]));
	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
  		illegal_cmd = (num_arg > 1);
  		if(!illegal_cmd){
  			shl.Background((num_arg == 0) ? -1 : atoi(args[1]));

  		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "history")) {
		illegal_cmd = (num_arg != 0);
		if (!illegal_cmd)
			shl.PrintHistory();
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		illegal_cmd = (num_arg > 1);
		if (!illegal_cmd)
			shl.Exit((num_arg == 0) ? "" : args[1]);

	} 
	else if(!strcmp(cmd, "kill"))
	{
		illegal_cmd = (num_arg != 2);
		if(!illegal_cmd)
			shl.Kill(args[1], args[2]);
	}
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmdString, num_arg + 1); //+1 for the command token
	 	return 0;
	}
	//if the built in command has wrong paramaters print an error message
	if (illegal_cmd == true){
		fprintf(stderr, "smash error: > \"%s\"\n", cmdString);
		return 1;
	}
	return 0;
}

//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string, length of the arguments array
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString, unsigned num_args)
{
	string command = __reconstruct_from_args(args);
	//only here check if the requested process should run in the background
	bool is_fg;
	if(!strcmp(args[num_args - 1], "&")){
		is_fg = false;
		args[num_args - 1] = NULL;
	}
	else is_fg = true;


	int pID;
    switch(pID = fork())
	{
    		case -1:
					// Add your code here (error)
					perror("Error forking:");
					exit(1);
        	case 0 :
                	// Child Process
               		setpgrp();
               		__register_child_handlers();
#ifdef DEBUG
               		sleep(5);
               		printf("performing...");
#endif
			        // Add your code here (execute an external command)
					if(execvp(args[0], args) < 0){
						cerr << "smash error:> \"" << command  << "\": "<< strerror(errno) << endl;
						exit(EXIT_FAILURE);
					}
					else exit(EXIT_SUCCESS); //not really executed if execvp runs correctly
			//pID = Child Process ID
			default:{
				// Father Process
				//wait for the child process to complete execution
				Shell& shl = Shell::SingInst();
				if (is_fg) {
					shl.SetForegroundProcess(pID);
					shl.AddJob(command.c_str(), pID);
					while(Shell::SingInst().IsForegroundLock()){pause();};
				}
				else{
					//this seems to be a code copying from few lines above,
					//but AddJob is quite complex and consumes CPU time, so the first AddJob
					//has to execute only after the foreground lock has been set
					shl.AddJob(command.c_str(), pID);
				}

				break;
			}
	}
}

//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize)
{
	char *args[MAX_ARG] = {(char*)"csh", (char*)"-f", (char*)"-c", lineSize, NULL}; unsigned num_args = 4;
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
    	ExeExternal(args, NULL, num_args);
    	return 0;
	} 
	return -1;
}

//**************************************************************************************
// function name: InitSmash
// Description: initializes the smash's structures and handlers
// Parameters: None
// Returns: void
//**************************************************************************************
void InitSmash(){
	//this will be effectively the 1st time the constructor of the static instance (singleton) will be called.
	//inializing all the 'global' structures and variables, and registring the signal handlers

	Shell::SingInst();
	return;
}
