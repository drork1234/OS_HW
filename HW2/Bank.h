/*
 * Bank.h
 *
 *  Created on: May 25, 2017
 *      Author: dror
 */

 /*
	Module Name : Bank
	Description : an implementation of a Bank. The bank holds a list of BankAccount pointers (dynamicaly allocated).
					The Bank will run 2 distinct threads - charging commissions from bank accounts & printing bank stats to stdout
	Main methods: 	1. Main - creates and runs said methods
					2. ChargeCommissions - runs as a different thread
					3. PrintBankStats - runs as a different thread
 */
#ifndef BANK_H_
#define BANK_H_

#include <iostream>
#include <vector>

#include "BankAccount.h"
#include "rwlock.h"
#include "Logger.h"

using namespace std;

//NOTE: deleting an account from the bank requires locking the whole data structure so
//there wouldn't be a fatal case where another thread is waiting to read values from the account that
//is being deleted, (otherwise a seg-fault)


/********************************************
// 	class name	: 	Bank
// 	Description	: 	A class that manages the access and traffic to the accounts of the bank.
//					The class holds a list of accounts and enables access and manipulation of said account
//					via class methods. These operations are protected by a read-write lock in order to assure thread safety
//					Said operations are utilized by the ATM instances that are allowed to access the accounts of the bank, but
//					must do so in a thread-safe way, hence the use of the read-write lock attached to the data structure that holds
//					the bank accounts
//					The bank deploys two independant threads.
//						1st thread charges commission from the bank's accounts every 3 seconds
//						2nd thread prints the status of the bank to stdout
//
//	Members		:	m_accounts	- a container that holds the all of the accounts belong to the bank
//					m_accs_lock	- a read-write lock that is attached to m_accounts container
//					m_logger	- an instance of Logger class, a thread-safe logger. The bank writes every message about the operations to this file
//					m_bank_balance - the balance of the bank. Raised by charging commission from the accounts
//					m_finished_atms - a thread-safe counter that notifies the bank about how many ATM's have finished their job (= done all of their operations).
//										when the counter reaches it's threashold, the bank stops its work
//					
//
//	Methods		:	Withdraw 		: withdraw an amount of money from a certain account
//					Deposit  		: deposit an amount of money to a certain account
//					Balance  		: return the balance from a certain account
//					RemoveAccount	: delete a certain account from the bank
//					OpenAccount		: open a new account in the bank
//					Transfer		: transfer money from one account to other account
*/
class Bank {
public:
	/********************************************
	// function name: 	Bank::Bank
	// Description	: 	Constructor.
	//					Initializes the bank's balance to 0, no accounts, logger to "log.txt", and the atm counter to 0.
	//					Also initializes to lock of the accounts' data structure.
	// Parameters	: 	None
	// Returns		: 	None
	// Exception	: 	None
	*/
	Bank();
	
	/********************************************
	// function name: 	Bank::~Bank
	// Description	: 	Destructor.
	// Parameters	: 	None
	// Returns		: 	None
	// Exception	: 	None
	*/
	~Bank();

public: //Main threads of the bank
	/********************************************
	// function name: 	Bank::ChargeCommissions
	// Description	: 	Charges commissions (with an interest rate of 2%-4%) from the bank's accounts every 3 seconds 
	//					Runs as an independant thread
	// Parameters	: 	None
	// Returns		: 	None
	// Exception	: 	None
	*/
	void ChargeCommissions();
	
	/********************************************
	// function name: 	Bank::PrintBankStats
	// Description	: 	Prints a snapshot of the banks' status (including stats of the accounts)
	//					Exclusive access is acquired via WriteLock (not really changing the status of the bank, but a write lock is an exclusive lock)
	//					Runs as an independant thread
	// Parameters	: 	None
	// Returns		: 	None
	// Exception	: 	None
	*/
	void PrintBankStats() const; //use WriteLock to acquire an exclusive lock the structure
	
	/********************************************
	// function name: 	Bank::Main
	// Description	: 	Spawns the two main threads of the bank (charging commissions & status printing). 
	//					Waits for the two threads to end (created as joinable)
	// Parameters	: 	None
	// Returns		: 	None
	// Exception	: 	None
	*/
	void Main();

public: //ATM supported methods of the bank
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
	bool RemoveAccount(int account_no, string password, int atm_id);
	
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
	bool OpenAccount(int account_no, string password, int balance, int atm_id);
	
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
	bool Deposit(int account_no, string password, int amount, int atm_id);
	
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
	bool Withdraw(int account_no, string password, int amount, int atm_id);
	
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
	bool Balance(int account_no, string password, int atm_id);
	
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
	bool Transfer(int account_no, string password, int account_target, int amount, int atm_id);

private:
	/********************************************
	// function name: 	Bank::__set_counter_top
	// Description	: 	Sets the number of ATM accessing the bank (used by ATM_manager at allocation)
	// Parameters	: 	top - number of atm's. This will be the counter's new top border.
	// Returns		: 	None
	// Exception	: 	None
	*/
	void __set_counter_top(int top){
		m_finished_atms.SetTop(top);
	}

	/********************************************
	// function name: 	Bank::__signal_finished
	// Description	: 	A message that is sent from a certain ATM to the bank,
	//					raises the number of finished ATM's by increasing the bank's internal thread safe counter
	// Parameters	: 	None
	// Returns		: 	None
	// Exception	: 	None
	*/
	void __signal_finished(){
		++m_finished_atms;
		return;
	}

private:
	vector<BankAccount*> m_accounts;
	mutable rwlock m_accs_lock;

	Logger m_logger;
	int m_bank_balance;
	thread_safe_counter m_finished_atms;

	friend class ATM_manager;
	friend class ATM;
};



#endif /* BANK_H_ */
