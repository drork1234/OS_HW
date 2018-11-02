/*
 * System.cpp
 *
 *  Created on: May 28, 2017
 *      Author: dror
 */

#include <exception>
#include <pthread.h>
#include "System.h"

#define NUM_MAIN_THREADS 2
typedef void* (*thread_fn)(void*);

//******************************System main POSIX threads************************************
/********************************************
// function name: 	run_bank
// Description	: 	Bank thread's routine. 
//					Runs Bank::Main
// Parameters	: 	arg - a void* to the Bank object
// Returns		: 	void*
// Exception	: 	None
*/
void* run_bank(void* arg){
	Bank* bank = reinterpret_cast<Bank*>(arg);
	bank->Main();
	pthread_exit((void*)0);
}

/********************************************
// function name: 	run_atm_manager
// Description	: 	ATM_manager thread's routine. 
//					Runs ATM_manager::Main
// Parameters	: 	arg - a void* to the ATM_manager object
// Returns		: 	void*
// Exception	: 	None
*/
void* run_atm_manager(void* arg){
	ATM_manager* manager = reinterpret_cast<ATM_manager*>(arg);
	manager->Main();
	pthread_exit((void*)0);
}


//***********************************************System API********************************************


/********************************************
// function name: 	System::System
// Description	: 	Constructor.
//					Initializes the Bank and the ATM_manager 
// Parameters	: 	atm_files - a list of file paths for the ATMs files
// Returns		: 	None
// Exception	: 	Propagates std::ifstream::failure from ATM::ATM ctor, if needed
*/
System::System(vector<string> const& atm_files) : m_bank(NULL), m_manager(NULL){
	try {
		m_bank = new Bank;
		m_manager = new ATM_manager(atm_files, m_bank);
	} catch (std::bad_alloc& e) {
		if (m_bank) delete m_bank;
		throw;
	}
	catch (std::ifstream::failure& e) {
		delete m_manager;
		delete m_bank;
		throw;
	}
}


/********************************************
// function name: 	System::~System
// Description	: 	Destructor
//					Releases all of the ATMs' and Bank's memory 
// Parameters	: 	None
// Returns		: 	None
// Exception	: 	None
*/
System::~System(){
	if (m_manager) 	delete m_manager;
	if (m_bank) 	delete m_bank;
}

/********************************************
// function name: 	System::Main
// Description	: 	Main method of the class. 
//					Creates 2 distinc threads, 1st thread runs Bank::Main, 2nd thread runs ATM_manager::Main
// Parameters	: 	None
// Returns		: 	None	
// Exception	: 	None
*/
void System::Main(){
	//create threads metadata
	pthread_t main_threads[NUM_MAIN_THREADS];
	thread_fn mains[NUM_MAIN_THREADS] = {run_bank, run_atm_manager};
	void* args[NUM_MAIN_THREADS] = {static_cast<void*>(m_bank), static_cast<void*>(m_manager)};
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	//create and run the threads
	for (unsigned i = 0; i < NUM_MAIN_THREADS; ++i)
		pthread_create(&main_threads[i], &attr, mains[i], args[i]);

	pthread_attr_destroy(&attr);

	//wait for the threads to finish
	for(unsigned i = 0; i < NUM_MAIN_THREADS; ++i)
		pthread_join(main_threads[i], NULL);

	return;
}
