/*
 * System.h
 *
 *  Created on: May 28, 2017
 *      Author: dror
 */

 
/*
	Module Name : System
	Description : An implementation of the system manager. Allocates and runs the main blocks of the program (Bank & ATM_manager)
	Main methods: 	1. Main - creates 2 different threads that run ATM_manager::Main & Bank::Main
 */
 
 
#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "Bank.h"
#include "ATM_manager.h"


/********************************************
// 	class name	: 	System
// 	Description	: 	A class that control the initialization of the whole bank system a.k.a The ATMs (ATM_manager) and the Bank
//
//	Members		:	manager - An ATM_manager, allocated on the heap
//					bank - A Bank object, allocated on the heap
//					
//	Methods		:	Main - ATM_manager::Main & Bank::Main on two distinct threads
*/
class System {
public:
	/********************************************
	// function name: 	System::System
	// Description	: 	Constructor.
	//					Initializes the Bank and the ATM_manager 
	// Parameters	: 	atm_files - a list of file paths for the ATMs files
	// Returns		: 	None
	// Exception	: 	Propagates std::ifstream::failure from ATM::ATM ctor, if needed
	*/
	System(vector<string> const& atm_files);
	
	/********************************************
	// function name: 	System::~System
	// Description	: 	Destructor
	//					Releases all of the ATMs' and Bank's memory 
	// Parameters	: 	None
	// Returns		: 	None
	// Exception	: 	None
	*/
	~System();
	
public: //API
	/********************************************
	// function name: 	System::Main
	// Description	: 	Main method of the class. 
	//					Creates 2 distinc threads, 1st thread runs Bank::Main, 2nd thread runs ATM_manager::Main
	// Parameters	: 	None
	// Returns		: 	None
	// Exception	: 	None
	*/
	void Main();

private: //do not allow the user to copy the object 
	System(System const&) : m_bank(NULL), m_manager(NULL){}

private:
	Bank* m_bank;
	ATM_manager* m_manager;
};


#endif /* SYSTEM_H_ */
