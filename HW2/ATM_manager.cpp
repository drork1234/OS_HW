/*
 * ATM_manager.cpp
 *
 *  Created on: May 27, 2017
 *      Author: dror
 */

#include "ATM_manager.h"
#include <fstream>
#include <exception>
#include <pthread.h>

//atm main thread method
void* ATM_main(void* patm){
	ATM* atm = (reinterpret_cast<ATM*>(patm));
	atm->Main();
	pthread_exit((void*)0);
}


/********************************************
// function name: 	ATM_manager::ATM_manager
// Description	: 	Constructor.
//					Initializes N ATMs 
// Parameters	: 	atm_files - a list of file paths for the ATMs files
//					bank - a reference to a Bank object, to be passed to the ATM contructor
// Returns		: 	None
// Exception	: 	Propagates std::ifstream::failure from ATM::ATM ctor, if needed
*/
ATM_manager::ATM_manager(vector<string> const& atm_files, Bank* bank){
	m_atms.reserve(atm_files.size());

	//create the atms
	for (unsigned i = 0; i < atm_files.size(); ++i) {
		//try to create a new ATM, catch the exception if file doesn't exist
		try {
			m_atms.push_back(new ATM(atm_files[i], bank, i + 1));
		}

		catch (std::ifstream::failure& e) {
			for (int j = m_atms.size() - 1; j >= 0; --j)
				delete m_atms[j];

			m_atms.clear();
			throw;
		}
	}

	//set the counter top to be the number of ATMs in the system
	bank->__set_counter_top(m_atms.size());
}

/********************************************
// function name: 	ATM_manager::~ATM_manager
// Description	: 	Destructor
//					Releases all of the ATMs' memory 
// Parameters	: 	None
// Returns		: 	None
// Exception	: 	None
*/
ATM_manager::~ATM_manager(){
	for (int i = (int)(m_atms.size() - 1); i >= 0; --i)
		delete m_atms[i];

	m_atms.clear();
}


/********************************************
// function name: 	ATM_manager::Main
// Description	: 	Main method of the class. 
//					Creates N new joinable threads, runs ATM::Main inside each one of them, and waits for them to finish
// Parameters	: 	None
// Returns		: 	None
// Exception	: 	None
*/
void ATM_manager::Main(){
	//initialize the threads metadata
	vector<pthread_t> atm_threads;
	atm_threads.resize(m_atms.size());
	pthread_attr_t attr_t;
	pthread_attr_init(&attr_t);
	pthread_attr_setdetachstate(&attr_t, PTHREAD_CREATE_JOINABLE);

	//create threads for the ATMs
	for (unsigned i = 0; i < atm_threads.size(); ++i)
		pthread_create(&atm_threads[i], &attr_t, ATM_main, static_cast<void*>(m_atms[i]));

	pthread_attr_destroy(&attr_t);

	//wait for all the threads to join the main thread
	for (unsigned i = 0; i < atm_threads.size(); ++i)
		pthread_join(atm_threads[i], NULL);

	return;
}
