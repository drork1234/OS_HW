

  /*
	Module Name : Logger
	Description : An implementation of a thread-safe logger
	Main methods: 	1. operator << - a template output operator, enables ease of use (cout-like)
 */
 
#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <fstream>

using namespace std;




/********************************************
// 	class name	: 	Logger
// 	Description	: 	An Implementation of a CSV logger
//
//	Members		:	m_log  : an ofstream object, the log file stream
//					
//	Methods		:	operator << - overloading this operator in order to support simple syntax (cout-like)
*/
class Logger {
public:
	//********************************************
	// function name: Logger::Logger
	// Description	: 	Construction of the logger class.
	//					Checks if file exists (else throws)
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
		m_log << data;
		return *this;
	}

	//same as above, but for endl support/overload
	Logger& operator << (ostream& (*pf)(ostream&)){
		m_log << pf;
		return *this;
	}

private:
	ofstream m_log;
};


#endif /* LOGGER_H_ */
