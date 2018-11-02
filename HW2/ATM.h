/*
 * ATM.h
 *
 *  Created on: May 27, 2017
 *      Author: dror
 */

#ifndef ATM_H_
#define ATM_H_

#include <fstream>
#include <string>
#include "Bank.h"

using namespace std;

typedef vector<vector<string>> parsed_file;

/********************************************
// 	class name	: 	ATM
// 	Description	: 	An implementation of an ATM service station. 
//
//	Members		:	m_atm_id - a unique identifir of the ATM
//					m_file_path - the file path where the ATM will read its operations from
//					m_bank_ref - a reference (pointer) to a Bank object, where the ATM will send it's requests
//					m_file_contents - a word-broken file. This is required since reading line by line from a file while executing with getline
//										causes data race conflicts, so the file is parsed at initialization (on the main thread), 
//										before ATM is sent to execution on a seperate thread
//					
//	Methods		:	Main - the Main thread of ATM in which the ATM will execute its commands according to the file contents
*/
class ATM {
public:	
	/********************************************
	// function name: 	ATM::ATM
	// Description	: 	Constructor.
	//					Check if file path is real, if yes - parses the file contents, if not - throws an ifstream::failure exception
	// Parameters	: 	file_path - the path to the commands file
	//					bank - a reference to a Bank object, where the ATM will send its requests
	// Returns		: 	None
	// Exception	: 	std::ifstream::failure in case file_path doesn't exist
	*/
	ATM(string file_path, Bank* bank_ref, int atm_id);

	/********************************************
	// function name: 	ATM::Main
	// Description	: 	Main method of the class. 
	//					Reads every 100 mili-sec a line from the parsed file contents, and sends a request to the bank 
	// Parameters	: 	None
	// Returns		: 	None
	// Exception	: 	None
	*/
	void Main();

private:
	int m_atm_id;
	string m_file_path; //No need for a lock, only reading and only from one source
	Bank* m_bank_ref;
	parsed_file m_file_contents; //since getline causes data race conflicts, parse the file at initialization, before sent to thread
};


#endif /* ATM_H_ */
