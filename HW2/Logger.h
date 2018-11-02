/*
 * Logger.h
 *
 *  Created on: May 26, 2017
 *      Author: dror
 */

  /*
	Module Name : Logger
	Description : An implementation of a thread-safe logger
	Main methods: 	1. operator << - a template output operator, enables ease of use (cout-like)
 */
 
#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <fstream>
#include "rwlock.h"

using namespace std;




/********************************************
// 	class name	: 	ATM_manager
// 	Description	: 	An Implementation of a POSIX thread safe logger
//
//	Members		:	m_log  : an ofstream object, the log file stream
//					m_lock : a rwlock object, the thread safe mechanism for protecting the m_log file
//					
//	Methods		:	operator << - overloading this operator in order to support simple syntax (cout-like)
*/
class Logger {
public:
	//********************************************
	// function name: Logger::Logger
	// Description	: 	Construction of the logger class.
	//					Checks if file exists (else throws), and initializes the internal lock 
	//					for thread safety
	// Parameters	: log_file : a string representing the path to the file
	// Returns		: None
	// Exception	: In case the file doesn't exist, throw an std::ifstream::failure error
	Logger(string m_log_file);
	
	//********************************************
	// function name: Logger::~Logger
	// Description	: Destructor of the logger class
	//					Closes the file safely and releases the resource after that
	// Parameters	: None
	// Returns		: None
	~Logger();

public:

	/********************************************
	// function name: 	ATM_manager::operator << 
	// Description	: 	Template out stream operator
	// Parameters	: 	data - the data to be written to the stream
	// Returns		: 	Logger& - a referece to this object
	// Exception	: 	None
	*/
	template <class T> Logger& operator << (const T& data){
		m_lock.WriteLock();
		//critical section, modifying logger state
		m_log << data;
		//end of critical section
		m_lock.WriteUnlock(false); //do not sleep before unlocking
		return *this;
	}

	//same as above, but for endl support/overload
	Logger& operator << (ostream& (*pf)(ostream&)){
		m_lock.WriteLock();
		m_log << pf;
		m_lock.WriteUnlock(); //do not sleep before unlocking
		return *this;
	}

private:
	ofstream m_log;
	mutable rwlock m_lock;
};


#endif /* LOGGER_H_ */
