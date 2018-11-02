
/*
 * Logger.cpp
 *
 *  Created on: May 26, 2017
 *      Author: dror
 */
#include "Logger.h"
#include <exception>
#include <sstream>

//********************************************
// function name: Logger::Logger
// Description	: 	Construction of the logger class.
//					Checks if file exists (else throws), and initializes the internal lock 
//					for thread safety
// Parameters	: log_file : a string representing the path to the file
// Returns		: None
// Exception	: In case the file doesn't exist, throw an std::ifstream::failure error
Logger::Logger(string log_file) : m_log(log_file, ofstream::out){
	if (!m_log.is_open()) {
		stringstream error;
		error << "log file " << log_file << " could not be opened!" << endl;
		throw ofstream::failure(error.str());
	}
	
	//output to the log file the header in a CSV form (',' seperated)
	m_log << "Page Number,Virtual Address,Physical Address,Page Fault,Swap,Evicted,Allocated Page Table Entries" << endl;
	return;
}


//********************************************
// function name: Logger::~Logger
// Description	: Destructor of the logger class
//					Closes the file safely and releases the resource after that
// Parameters	: None
// Returns		: None
Logger::~Logger(){
	m_log.close();
}
