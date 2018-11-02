/*
 * BankBankAccount.h
 *
 *  Created on: May 24, 2017
 *      Author: dror
 */
 
 /*
	Module Name : BankAccount
	Description : an implementation of a Bank Account. 
					The account will hold the account number, password and balance. 
					In order to ensure thread-safety, the account will hold a rwlock object
	Main methods: 	1. Withdraw - withdraw amount of money from an account
					2. Deposit - deposit amount of money to an account
					3. Balance - report the amount of money inside the account
 */

#ifndef BankAccount_H_
#define BankAccount_H_

#include <stdlib.h>
#include <limits.h>
#include <string>
#include "rwlock.h"

using namespace std;

/********************************************
// 	class name	: 	BankAccount
//	Members		:	m_account_number 	- the number of the account (unique). Integer
//					m_password			- the password of the account. std::string.
//					m_balance			- the amount of money at the bank account, currently. Integer.
//					m_rwlock			- the thread-lock protecting the balance of the account. Sleeps for one second in case told so before unlocking
//
//	Methods		:	Withdraw : withdraw an amount of money from the account
//					Deposit  : deposit an amount of money to the account
//					Balance  : return the balance of the account
//					AccountNumber : return the account number
//					Password : return the password of the account
*/
class BankAccount {
public:
	/********************************************
	// function name: 	BankAccount::BankAccount
	// Description	: 	Constructor.
	//					Initializes the balance to the given balance + the lock
	// Parameters	: 	account_no - the account number (int)
	//					password   - the password of the account (std::string)
	//					balance    - the balance of the account (int)
	// Returns		: 	None
	// Exception	: 	None
	*/
	BankAccount(int account_no, string password ,int balance);


public://API
	/********************************************
	// function name: 	BankAccount::Withdraw
	// Description	: 	Withdraw an amount of money from the account
	//					Fails incase amount > balance.
	// Parameters	: 	amount 	 - the amount to be withrawn from the account (int)
	//					is_sleep - tells the internal lock to sleep (or not)
	// Returns		: 	The account's new balance (after the operation). 
	// Exception	: 	None
	// Thread-safety:	Yes
	*/
	int Withdraw(int amount, bool is_sleep = true);

	/********************************************
	// function name: 	BankAccount::Deposit
	// Description	: 	Deposit an amount of money to the account
	// Parameters	: 	amount 	 - the amount to be deposited the the account (int)
	//					is_sleep - tells the internal lock to sleep (or not)
	// Returns		: 	The account's new balance (after the operation).
	// Exception	: 	None
	// Thread-safery:	Yes
	*/
	int Deposit(int amount, bool is_sleep = true);

	/********************************************
	// function name: 	BankAccount::Balance
	// Description	: 	Returns the balance of the account
	// Parameters	: 	is_sleep - tells the internal lock to sleep (or not)
	// Returns		: 	The balance of the account (int)
	// Exception	: 	None
	// Thread-safery:	Yes
	*/
	int Balance(bool is_sleep = true) const;

	/********************************************
	// function name: 	BankAccount::AccountNumber
	// Description	: 	Returns the account's number
	// Parameters	: 	None
	// Returns		: 	The account's number (int)
	// Exception	: 	None
	// Thread-safery:	Yes
	*/
	int AccountNumber() const;

	/********************************************
	// function name: 	BankAccount::Password
	// Description	: 	Returns the account's password
	// Parameters	: 	None
	// Returns		: 	The account's password (std::string)
	// Exception	: 	None
	// Thread-safery:	Yes
	*/
	string Password() const;


private:
	int m_account_number;
	string m_password;
	int m_balance;
	mutable rwlock m_rwlock; //for it to be changed (locked/unlocked) in const methods (state is defined by balance)
};



#endif /* BANKBankAccount_H_ */
