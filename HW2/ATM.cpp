/*
 * ATM.cpp
 *
 *  Created on: May 27, 2017
 *      Author: dror
 */
#include <sstream>
#include <unistd.h>
#include <iterator>
#include <algorithm>
#include "ATM.h"
#include "defs.h"

typedef vector<string> line_tokens;

/********************************************
// function name: 	tokenize
// Description	: 	Breaks a line into tokens 
//					
// Parameters	: 	file_path - the path to the path
//					file__ - a reference to a parsed_file object, where the file contents will be filled
// Returns		: 	None
// Exception	: 	None
*/
void tokenazie(string line, vector<string>& tokens){
	tokens.clear();
	stringstream ss(line);
	string token;
	while(ss >> token) tokens.push_back(token);
}


/********************************************
// function name: 	__parse_file
// Description	: 	Parses a file to vector on broken lines (vectors ot tokens)
//					
// Parameters	: 	file_path - the path to the path
//					file__ - a reference to a parsed_file object, where the file contents will be filled
// Returns		: 	None
// Exception	: 	None
*/
void __parse_file(string const& file_path, parsed_file& file__){
	file__.clear();
	ifstream file_stream(file_path);
	if(!file_stream.is_open()) return;
	
	//count the number of lines inside the file
	file_stream.unsetf(std::ios_base::skipws);
	unsigned num_file_lines = std::count(istream_iterator<char>(file_stream), istream_iterator<char>(), '\n'); // count the number of lines inside the file
	
	//reset fstream settings
	file_stream.clear();
	file_stream.setf(std::ios_base::skipws);
	file_stream.seekg(0, ios::beg);

	//parse the file
	file__.reserve(num_file_lines);
	string line;

	//read the file line by line
	while (getline(file_stream, line)) {
		vector<string> line_tokens;
		
		//break the line into tokens
		tokenazie(line, line_tokens);
		
		//push the tokens as a new entry
		file__.push_back(line_tokens);
	}

	file_stream.close();
}

//helper

//*****************************************************************ATM class API**********************************************************


/********************************************
// function name: 	ATM::ATM
// Description	: 	Constructor.
//					Check if file path is real, if yes - parses the file contents, if not - throws an ifstream::failure exception
// Parameters	: 	file_path - the path to the commands file
//					bank - a reference to a Bank object, where the ATM will send its requests
// Returns		: 	None
// Exception	: 	std::ifstream::failure in case file_path doesn't exist
*/
ATM::ATM(string file_path, Bank* bank_ref, int atm_id) : m_atm_id(atm_id), m_file_path(file_path), m_bank_ref(bank_ref){
	ifstream file(file_path);
	if (!file.is_open()) {
		stringstream error;
		error << __func__ << " LINE " <<  __LINE__ << ": file at path " << file_path << " does not exist" << endl;
		throw std::ifstream::failure(error.str());
	}
	file.close();

	//parse the file
	__parse_file(m_file_path, m_file_contents);
}



/********************************************
// function name: 	ATM::Main
// Description	: 	Main method of the class. 
//					Reads every 100 mili-sec a line from the parsed file contents, and sends a request to the bank 
// Parameters	: 	None
// Returns		: 	None
// Exception	: 	None
*/
void ATM::Main(){
	//read contents of the tokenized file
	for(unsigned i = 0; i < m_file_contents.size(); ++i){
		line_tokens& tokens = m_file_contents.at(i);
		char op = tokens[0][0];
		int account_no = atoi(tokens[1].c_str());
		string password = tokens[2];
		switch (op) {
			case 'O':{
				int amount = atoi(tokens[3].c_str());
				m_bank_ref->OpenAccount(account_no, password, amount, m_atm_id);
				break;
			}

			case 'D': {
				int amount = atoi(tokens[3].c_str());
				m_bank_ref->Deposit(account_no, password, amount, m_atm_id);
				break;
			}

			case 'W':{
				int amount = atoi(tokens[3].c_str());
				m_bank_ref->Withdraw(account_no, password, amount, m_atm_id);
				break;
			}

			case 'B':{
				m_bank_ref->Balance(account_no, password, m_atm_id);
				break;
			}

			case 'Q':{
				m_bank_ref->RemoveAccount(account_no, password, m_atm_id);
				break;
			}

			case 'T':{
				int account_target = atoi(tokens[3].c_str());
				int amount = atoi(tokens[4].c_str());

				m_bank_ref->Transfer(account_no, password, account_target, amount, m_atm_id);
				break;
			}
			default:
				break;
		}

		//sleep for 0.1 seconds, then operate
		usleep(HUNDRED_MILI_SEC);
	}

	//signal to the bank that commands processing has finished
	m_bank_ref->__signal_finished();
	return;
}
