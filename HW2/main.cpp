/*
 * main.cpp
 *
 *  Created on: Apr 20, 2017
 *      Author: dror
 */
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <typeinfo>
#include <vector>
#include "System.h"


#define ATM_FILES_START 2

using namespace std;

typedef enum {NOT_ENOUGH_FILES = -1, BAD_FILE = -2, BAD_ALLOC = -3} ERR_;
typedef void* (*thread_fn)(void*);

//helper function, reorganizes the main arguments passed from the command line into managable (iterable) structure
void __create_atm_files(int num_atms, char** argv, vector<string>& atm_files){
	atm_files.clear();
	atm_files.reserve(num_atms);
	for (unsigned i = 0; i < atm_files.capacity(); ++i)
		atm_files.push_back(argv[i]);
}


//main thread
int main(int argc, char** argv) {
	if(argc == 1){
		cerr << "illegal commands" << endl;
		return NOT_ENOUGH_FILES;
	}
	int num_atms = atoi(argv[1]);
	if (num_atms != argc - 2){ // -2 means the name of the program + the string that tells the number of ATMs
		//at this point of the program the cerr global stream is being written only from the main thread
		//because there are not any other threads attached to the program, so no need to protect cerr
		cerr << "illegal arguments" << endl;

		return NOT_ENOUGH_FILES;
 	}

	//create the atm files list
	vector<string> atm_files;
	__create_atm_files(num_atms, argv + ATM_FILES_START, atm_files);

	//NOTICE: the constructor of the bank and the ATM_manager are spawned from the main thread
	//but each one of them launches independent threads that deploy the ATMs and the bank methods

	try {
		//create the system
		System sys(atm_files);

		//run the entire system
		sys.Main();

	} catch(std::exception& e){ //Catching ifstream::failure (in case of non-existent input file) and std::bad_aloc (in case of system allocation error at initialization)
		cerr << e.what() << endl;
		if(typeid(e) == typeid(std::ifstream::failure&))
			return BAD_FILE;

		else if (typeid(e) == typeid(std::bad_alloc&))
			return BAD_ALLOC;
	}
	return EXIT_SUCCESS;
}

