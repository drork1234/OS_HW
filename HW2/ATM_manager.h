/*
 * ATM_manager.h
 *
 *  Created on: May 27, 2017
 *      Author: dror
 */
 
 /*
	Module Name : ATM_manager
	Description : A wrapper around an std::vector<ATM*> that allocates and frees N ATMs.
					Also, the manager will spawn N threads, each thread for each ATM.
	Main methods: 	1. ATM_manager::ATM_manager - allocates N ATMs
					2. ATM_manager::Main - creates and runs N different threads (a thread for each ATM)
 */

#ifndef ATM_MANAGER_H_
#define ATM_MANAGER_H_


#include "ATM.h"
#include "Bank.h"
#include <vector>
#include <string>

using namespace std;

/********************************************
// 	class name	: 	ATM_manager
// 	Description	: 	A class that manages the allocation and operation of N ATM machines
//					The class spawns N independant threads, each one operates a single unique ATM, using ATM::Main()
//
//	Members		:	m_atms : a container that holds N dynamically allocated ATMs
//					
//	Methods		:	Main - the Main thread of ATM_manager that creates another N new joinable threads that operate all the ATMs
*/
class ATM_manager {
public:
	/********************************************
	// function name: 	ATM_manager::ATM_manager
	// Description	: 	Constructor.
	//					Initializes N ATMs 
	// Parameters	: 	atm_files - a list of file paths for the ATMs files
	//					bank - a reference to a Bank object, to be passed to the ATM contructor
	// Returns		: 	None
	// Exception	: 	Propagates std::ifstream::failure from ATM::ATM ctor, if needed
	*/
	ATM_manager(vector<string> const& atm_files, Bank* bank);
	
	/********************************************
	// function name: 	ATM_manager::~ATM_manager
	// Description	: 	Destructor
	//					Releases all of the ATMs' memory 
	// Parameters	: 	None
	// Returns		: 	None
	// Exception	: 	None
	*/
	~ATM_manager();

	/********************************************
	// function name: 	ATM_manager::Main
	// Description	: 	Main method of the class. 
	//					Creates N new joinable threads, runs ATM::Main inside each one of them, and waits for them to finish
	// Parameters	: 	None
	// Returns		: 	None
	// Exception	: 	None
	*/
	void Main();
	
private:
	vector<ATM*> m_atms;
};




#endif /* ATM_MANAGER_H_ */
