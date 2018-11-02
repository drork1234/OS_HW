/*
 * Bank.cpp

 *
 *  Created on: May 21, 2017
 *      Author: dror
 */
#include <algorithm>
#include <numeric>
#include <math.h>
#include <random>
#include <ctime>
#include <sstream>
#include <pthread.h>
#include <unistd.h>
#include <iomanip>
#include "Bank.h"
#include "defs.h"


#define HIGHEST_INTEREST 0.04
#define LOWEST_INTEREST 0.02
#define NUM_BANK_INDEPENDENT_THREADS 2

typedef void* (*thread_fn)(void*);


//**************************************Thread functions***************************
void* print_bank_stats(void* arg){
	Bank& bank = *reinterpret_cast<Bank*>(arg);
	bank.PrintBankStats();
	pthread_exit((void*)0);
}

void* charge_commissions(void* arg){
	Bank& bank = *reinterpret_cast<Bank*>(arg);
	bank.ChargeCommissions();
	pthread_exit((void*)0);
}

//***************************************Helper Functors***************************

/********************************************
// function name	: 	CompAccs
// Description	: 	a functor that helps the sorting process according to accounts' account numbers
// Members		: 	None
// Methods		: 	operator() - makes the object CALLABLE. returns bool - TRUE if fst account number is lesser than sec account number
*/
bool CompAccs(BankAccount* fst, BankAccount* sec){
	return fst->AccountNumber() < sec->AccountNumber();
}


/********************************************
// class name	: 	FindAccount
// Description	: 	a functor that helps to find a bank account according to its account number
// Members		: 	m_account_number - the number of the account that find_if tries to find
// Methods		: 	operator() - makes the object CALLABLE. returns bool - TRUE if looked up account number equals account[i]'s account number
*/
class FindAccount {
public:
	FindAccount(int account_no) :
			m_account_number(account_no) {
	}

	bool operator()(BankAccount* paccount) {
		return m_account_number == paccount->AccountNumber();
	}

private:
	int m_account_number;
};



/********************************************
// class name	: 	AccumulateCommision
// Description	: 	a functor that helps to accumulate the bank total commission from the bank accounts and reduces the charged amount from the account's balance
// Members		: 	m_commision_interest - the interest according to which the amount of commission will be charged from the accounts
//					m_logger - a thread-safe logger where the action will be reported
// Methods		: 	operation() - returns the sum of the current amount of commission + charged commision from account[i]
*/
class AccumulateCommision {
public:
	
	AccumulateCommision(float interest, Logger& logger) :
			m_commision_interest(interest), m_logger(logger) {
	}

	int operator()(int sum, BankAccount* paccount) {
		//calculate the commission charged from the account
		int commision = static_cast<int>(roundf(paccount->Balance(false) * m_commision_interest)); //do not sleep, just get the data
		
		//withdraw the commission from the account
		paccount->Withdraw(commision, false); // do not sleep

		//print the message to the log
		stringstream msg;
		msg 	<< "Bank: commissions of " << (int)roundf(100*m_commision_interest)
				<< " % were charged, the bank gained " << commision
				<< " $ from account " << paccount->AccountNumber() << std::endl;

		m_logger << msg.str(); //thread-safe logging
		return sum + commision;
	}

private:
	float m_commision_interest;
	Logger& m_logger;
};
//*********************************************************************************

/********************************************
// function name: 	Bank::Bank
// Description	: 	Constructor.
//					Initializes the bank's balance to 0, no accounts, logger to "log.txt", and the atm counter to 0.
//					Also initializes to lock of the accounts' data structure.
// Parameters	: 	None
// Returns		: 	None
// Exception	: 	None
*/
Bank::Bank() : m_accs_lock(ONE_SEC), m_logger("./log.txt"), m_bank_balance(0) {
	m_accounts.clear();

}

/********************************************
// function name: 	Bank::~Bank
// Description	: 	Destructor.
// Parameters	: 	None
// Returns		: 	None
// Exception	: 	None
*/
Bank::~Bank() {
	m_accs_lock.WriteLock();

	for (unsigned i = 0; i < m_accounts.size(); ++i)
		delete m_accounts[i];

	m_accounts.clear();

	m_accs_lock.WriteUnlock(false);
	
}

/********************************************
// function name: 	Bank::Main
// Description	: 	Spawns the two main threads of the bank (charging commissions & status printing). 
//					Waits for the two threads to end (created as joinable)
// Parameters	: 	None
// Returns		: 	None
// Exception	: 	None
*/
void Bank::Main(){
	pthread_t bank_threads[NUM_BANK_INDEPENDENT_THREADS];
	thread_fn bank_funcs[NUM_BANK_INDEPENDENT_THREADS] = {print_bank_stats, charge_commissions};

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	//function pointer array of bank threads

	//create the threads
	for(unsigned i = 0; i < NUM_BANK_INDEPENDENT_THREADS; ++i)
		pthread_create(&bank_threads[i], &attr, bank_funcs[i], (void*)this);

	pthread_attr_destroy(&attr);

	for(unsigned i = 0; i < NUM_BANK_INDEPENDENT_THREADS; ++i)
		pthread_join(bank_threads[i], NULL);
	//return to main thread
	return;
}

/********************************************
// function name: 	Bank::ChargeCommissions
// Description	: 	Charges commissions (with an interest rate of 2%-4%) from the bank's accounts every 3 seconds 
//					Runs as an independant thread
// Parameters	: 	None
// Returns		: 	None
// Exception	: 	None
*/
void Bank::ChargeCommissions() {
	std::srand(std::time(NULL));

	while(true){
		//create a new interest rate between 0.02 and 0.04
		float interest = (HIGHEST_INTEREST - LOWEST_INTEREST) * fabsf(static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) + LOWEST_INTEREST;
		
		m_accs_lock.WriteLock();
		//charge and accumulate the commission from all the bank accounts
		int tot_commision = accumulate(m_accounts.begin(), m_accounts.end(), 0, AccumulateCommision(interest, m_logger));
		//int tot_commision = 0;
		m_bank_balance += tot_commision;

		m_accs_lock.WriteUnlock(false);

		//check if atm's have finished their work. If yes, finish execution
		if(m_finished_atms.HasReachedTop())
			return;


		//sleep for three seconds
		usleep(THREE_SEC);

	}
}

/********************************************
// function name: 	Bank::PrintBankStats
// Description	: 	Prints a snapshot of the banks' status (including stats of the accounts)
//					Exclusive access is acquired via WriteLock (not really changing the status of the bank, but a write lock is an exclusive lock)
//					Runs as an independant thread
// Parameters	: 	None
// Returns		: 	None
// Exception	: 	None
*/
void Bank::PrintBankStats() const {

	while(true){
		m_accs_lock.WriteLock(); //NOTE: write lock acquires an exclusive lock on the container, so no account will be changed by other threads

		//sort the bank accounts according to their account id
		BankAccount** _begin = &m_accounts[0];
		sort(_begin, _begin + m_accounts.size(), CompAccs);
		
		printf("\033[H\033[J");   //clear the screen
		printf("\033[1;1H"); //move the cursor to the top left corner of the screen
		cout << "Current Bank Status" << endl;
		
		//loop all over the accounts and print their stats
		for (unsigned i = 0; i < m_accounts.size(); ++i) {
			//print the account's stats
			auto* account = m_accounts[i];
			cout << "Account " << account->AccountNumber() << ": Balance - "
					<< account->Balance(false) << " $ ," << "Account Password - " //do not sleep while getting information
					<< account->Password() << endl;
		}

		cout << "The Bank has " << m_bank_balance << " $" << endl;

		m_accs_lock.WriteUnlock(false);
		
		//sleep for half a second
		//check if atm's have finished their work. If yes, finish execution
		if(m_finished_atms.HasReachedTop())
			return;

		usleep(HALF_SEC);
	}

}


//*******************************API for usage by ATMs************************************************************************

/********************************************
// function name: 	Bank::OpenAccount
// Description	: 	Open a new account in the bank
// Parameters	: 	account_no - the account number
//					password - the password of the account
//					balance - the beginning balance of the new account
//					atm_id - the id of the atm that requested this operations
// Returns		: 	In case the account number addresses an account that already exists or password is incorrect, return false
//					Else return true
// Exception	: 	None (exits if std::bad_alloc occurs)
*/
bool Bank::OpenAccount(int account_no, string password, int balance, int atm_id) {
	//Acquire an exclusive lock over the accounts' structure because this operation changes it's internal structure
	m_accs_lock.WriteLock();
	
	//find if there's already an account with the account_no number
	auto found_account = find_if(m_accounts.begin(), m_accounts.end(), FindAccount(account_no));

	//if there is already an account with the same id, return false
	if (m_accounts.end() != found_account) {
		m_accs_lock.WriteUnlock(true);//sleep for a second

		//write to the log
		stringstream error;
		error << "Error " << atm_id << ": Your transaction failed - account with the same id exists" << endl;
		m_logger << error.str(); //thread safe logging

		return false; //account already exists
	}

	//else push a new account to the bank
	try {
		m_accounts.push_back(new BankAccount(account_no, password, balance));

	}
	//bad alloc handling
	catch (bad_alloc& e) {
		cerr << "Bank Account allocation failed!" << endl;
		this->~Bank();
		exit(EXIT_FAILURE);
	}

	m_accs_lock.WriteUnlock(true); //sleep for a second before unlocking

	//Post operation
	
	//write to the log
	stringstream msg;
	msg << atm_id << ": New account id is " << account_no << " with password " << password << " and initial balance " << balance << endl;

	m_logger << msg.str(); //thread-safe logging
	return true;
}


/********************************************
// function name: 	Bank::RemoveAccount
// Description	: 	Remove an account from the bank
// Parameters	: 	account_no - the account number
//					password - the password of the account
//					atm_id - the id of the atm that requested this operations
// Returns		: 	In case the account number addresses an account that doesn't exist or password is incorrect, return false
//					Else return true
// Exception	: 	None
*/
bool Bank::RemoveAccount(int account_no, string password, int atm_id) {
	//Acquire an exclusive lock over the accounts' structure because this operation changes it's internal structure
	m_accs_lock.WriteLock();
	bool password_correct = true;
	
	//find the account according to it's ID
	auto found_account = find_if(m_accounts.begin(), m_accounts.end(), FindAccount(account_no));

	//if account wasn't found
	if (found_account == m_accounts.end()) {
		m_accs_lock.WriteUnlock(true); //sleep for a second before unlocking the structure

		//post operation - write to log
		//write 'account not-existent' error to log
		stringstream error;
		error << "Error " << atm_id << ": Your transaction failed - account with the same id exists" << endl;
		m_logger << error.str(); //thread safe logging

		return false;
	}
	//check if password is correct
	password_correct = (*found_account)->Password() == password;
	int balance = (*found_account)->Balance(false); //do not sleep before unlocking
	if(password_correct){
		//if password is correct, remove the account
		delete (*found_account); // delete the account from the heap
		m_accounts.erase(found_account); //delete the iterator
	}
	m_accs_lock.WriteUnlock(true); //sleep for a second before unlocking the structure

	//post operation - write to the log
	stringstream msg;
	if(password_correct)
		msg << atm_id << ": Account " << account_no << " is now closed. Balance was " << balance << endl;
	else
		msg << "Error " << atm_id << ": Your transaction failed - password for account id " << account_no << " is incorrect" << endl;

	m_logger << msg.str();//thread-safe logging

	return password_correct;
}

/********************************************
// function name: 	Bank::Deposit	
// Description	: 	Deposit money to a certain account
// Parameters	: 	account_no - the account number
//					password - the password of the account
//					amount - the amount of money to be deposited
//					atm_id - the id of the atm that requested this operations
// Returns		: 	In case the account number addresses an account that doesn't exist or password is incorrect, return false
//					Else return true
// Exception	: 	None 
*/
bool Bank::Deposit(int account_no, string password, int amount, int atm_id) {
	m_accs_lock.ReadLock();
	auto found_account = find_if(m_accounts.begin(), m_accounts.end(), FindAccount(account_no));
	if (m_accounts.end() == found_account) {
		m_accs_lock.ReadUnlock(true); //sleep for one second

		//write to the log
		stringstream error;
		error << "Error " << atm_id << ": Your transaction failed - account with the same id exists" << endl;
		m_logger << error.str(); //thread safe logging
		return false;
	}

	int new_balance = 0;
	bool password_correct = (*found_account)->Password() == password;
	if (password_correct){
		new_balance = (*found_account)->Deposit(amount,true); //sleep for one second, true -> sleep
		m_accs_lock.ReadUnlock(false); //do not sleep after sleeping after depositing
	}
	else m_accs_lock.ReadUnlock(true); //sleep for one second

	//POST OPERATION: write to the log

	stringstream msg;
	if (password_correct)
		msg << atm_id << ": Account " << account_no << " new balance is " << new_balance << " after " << amount << "$ was deposited" << endl;
	else
		msg << "Error " << atm_id << ": Your transaction failed - password for account id " << account_no << " is incorrect" << endl;

	m_logger << msg.str(); //thread-safe logging

	return password_correct;
}

/********************************************
// function name: 	Bank::Withdraw
// Description	: 	Withdraw money from a certain account
// Parameters	: 	account_no - the account number
//					password - the password of the account
//					amount - the amount of money to be withdrawn
//					atm_id - the id of the atm that requested this operations
// Returns		: 	In case the account number addresses an account that doesn't exist or password is incorrect or withdrawl failed, return false
//					Else return true
// Exception	: 	None
*/	
bool Bank::Withdraw(int account_no, string password, int amount, int atm_id) {
	m_accs_lock.ReadLock();
	//find the account
	auto found_account = find_if(m_accounts.begin(), m_accounts.end(), FindAccount(account_no));
	
	//if account wasn't found
	if (m_accounts.end() == found_account) {
		m_accs_lock.ReadUnlock(true); //sleep for one second before unlocking

		//TODO: ADD log writing here
		stringstream error;
		error << "Error " << atm_id << ": Your transaction failed - account with the same id exists" << endl;
		m_logger << error.str(); //thread safe logging
		return false;
	}

	bool password_correct = (*found_account)->Password() == password;
	int balance = 0;
	if (password_correct){
		balance = (*found_account)->Withdraw(amount, true); //sleep for one second before unlocking the account


		m_accs_lock.ReadUnlock(false); //do not sleep
	}

	else m_accs_lock.ReadUnlock(true); //sleep for a second

	//post operation: Write to log
	//TODO: ADD log writing here
	stringstream msg;
	if (password_correct){
		if (balance > -1)
			msg << atm_id << ": Account " << account_no << " new balance is " << balance << " after " << amount << "$ was withdrawn" << endl;
		else
			msg << "Error: " << atm_id << ": Your transaction failed - account id " << account_no << " balance is lower than " << amount << endl;
	}
	else
		msg << "Error: " << atm_id << ": Your transaction failed - password for account id " << account_no << " is incorrect" << endl;

	m_logger << msg.str(); //thread-safe logging

	return password_correct && (balance > -1);
}

/********************************************
// function name: 	Bank::Balance
// Description	: 	Get the balance of a certain account
// Parameters	: 	account_no - the account number
//					password - the password of the account
//					atm_id - the id of the atm that requested this operations
// Returns		: 	In case the account number addresses an account that doesn't exist or password is incorrect, return false
//					Else return true
// Exception	: 	None
*/
bool Bank::Balance(int account_no, string password,int atm_id)  {
	m_accs_lock.ReadLock();
	//find the account
	auto found_account = find_if(m_accounts.begin(), m_accounts.end(), FindAccount(account_no));
	
	//if account wasn't found
	if (m_accounts.end() == found_account) {
		m_accs_lock.ReadUnlock(true); //sleep for one second

		//TODO: ADD log writing here
		stringstream error;
		error << "Error " << atm_id << ": Your transaction failed - account with the same id exists" << endl;
		m_logger << error.str(); //thread safe logging
		return false;
	}
	//check if password is correct
	bool password_correct = (*found_account)->Password() == password;
	int balance;
	if (password_correct){
		balance = (*found_account)->Balance(true); //sleep for one second before unlocking the account
		m_accs_lock.ReadUnlock(false); //already slept in the balance operation
	}
	else m_accs_lock.ReadUnlock(true); //sleep for one second

	//POST process:
	//TODO: ADD log writing here
	stringstream msg;
	if(password_correct)
		msg << atm_id << ": Account " << account_no << " balance is " << balance << endl;
	else
		msg << "Error " << atm_id << ": Your transaction failed - password for account id " << account_no << " is incorrect" << endl;

	m_logger << msg.str();
	return password_correct;
}

/********************************************
// function name: 	Bank::Transfer	
// Description	: 	Transfer money from a certain account to another
// Parameters	: 	account_no - the account number
//					password - the password of the account
//					account_target - the target account number
//					amount - the amount of money to be transfered
//					atm_id - the id of the atm that requested this operations
// Returns		: 	In case the account number addresses an account that doesn't exist or password is incorrect or target account wasn't found, return false
//					Else return true
// Exception	: 	None
*/
bool Bank::Transfer(int account_no, string password, int account_target, int amount, int atm_id) {
	m_accs_lock.ReadLock();
	//find the account
	auto found_account = find_if(m_accounts.begin(), m_accounts.end(), FindAccount(account_no));

	if (m_accounts.end() == found_account) {
		m_accs_lock.ReadUnlock(true); //sleep for one second

		//TODO: ADD log writing here
		stringstream error;
		error << "Error " << atm_id << ": Your transaction failed - account with the same id exists" << endl;
		m_logger << error.str(); //thread safe logging
		return false;
	}

	//find the target account
	auto found_target = find_if(m_accounts.begin(), m_accounts.end(), FindAccount(account_target));
	
	//if target account wasn't found
	if (m_accounts.end() == found_target) {
		m_accs_lock.ReadUnlock(true); //sleep for one second
		//TODO: ADD log writing here

		stringstream error;
		error << "Error " << atm_id << ": Your transaction failed - account id " << account_target << " does not exist" << endl;
		m_logger << error.str(); //thread safe logging
		return false;
	}

	//check the password of the account
	int balance, tar_balance;
	bool password_correct = (*found_account)->Password() == password;

	if(password_correct) //if password is correct, try to withdraw the money
		balance = (*found_account)->Withdraw(amount, true); //sleep for one second
	if(balance > -1){ //if withdrawl succeeded, deposit the amount
		tar_balance = (*found_target)->Deposit(amount, false);
		m_accs_lock.ReadUnlock(false);//do not sleep after transfer, already slept while transfer
	}
	//sleep before unlocking
	else m_accs_lock.ReadUnlock(true); // sleep for one second


	//POST opration:
	//TODO: write to log file here
	stringstream msg;
	if (password_correct) {
		if(balance > -1)
			msg << atm_id << "Transfer " << amount << " from account " << account_no << " to account " << account_target
					<< " new account balance is " << balance << " new target account balance is " << tar_balance << endl;
		else
			msg << "Error " << atm_id << ": Your transaction failed - account id " << account_no << " balance is lower than" << amount << endl;
	}
	else
		msg << "Error " << atm_id << ": Your transaction failed - password for account id " << account_no << " is incorrect" << endl;

	m_logger << msg.str(); //thread save logging

	return password_correct && (balance > -1);
}

