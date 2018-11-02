/*
 * BankBankAccount.cpp
 *
 *  Created on: May 24, 2017
 *      Author: dror
 *
 *	An implementation of the BankAccount class
 */

#include "BankAccount.h"
#include "defs.h"

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
BankAccount::BankAccount(int account_no, string password ,int balance) : m_account_number(account_no), m_password(password) ,m_balance(balance), m_rwlock(ONE_SEC) {

}

/********************************************
// function name: 	BankAccount::Withdraw
// Description	: 	Withdraw an amount of money from the account
//					Fails incase: amount > balance.
// Parameters	: 	amount 	 - the amount to be withrawn from the account (int)
//					is_sleep - tells the internal lock to sleep (or not)
// Returns		: 	The account's new balance (after the operation), if succeeded. -1 if fails. 
// Exception	: 	None
// Thread-safety:	Yes
*/
int BankAccount::Withdraw(int amount, bool is_sleep) {

	m_rwlock.WriteLock();
	//critical section
	bool cond = amount < m_balance;
	if(cond)
		m_balance -= amount;
	
	int new_balance = m_balance;
	//end of critical section
	m_rwlock.WriteUnlock(is_sleep); //sleep for one second if told so

	return cond ? new_balance : -1;
}

/********************************************
// function name: 	BankAccount::Deposit
// Description	: 	Deposit an amount of money to the account
// Parameters	: 	amount 	 - the amount to be deposited the the account (int)
//					is_sleep - tells the internal lock to sleep (or not)
// Returns		: 	The account's new balance (after the operation).
// Exception	: 	None
// Thread-safery:	Yes
*/
int BankAccount::Deposit(int amount, bool is_sleep) {

	m_rwlock.WriteLock();
	//critical section
	m_balance += amount;
	int new_balance = m_balance;
	//end of critical section
	m_rwlock.WriteUnlock(is_sleep);

	return new_balance;
}


/********************************************
// function name: 	BankAccount::Balance
// Description	: 	Returns the balance of the account
// Parameters	: 	is_sleep - tells the internal lock to sleep (or not)
// Returns		: 	The balance of the account (int)
// Exception	: 	None
// Thread-safery:	Yes
*/
int BankAccount::Balance(bool is_sleep) const {
	int c;
	m_rwlock.ReadLock();
	//critical section
	c = m_balance;
	//end of critical section

	m_rwlock.ReadUnlock(is_sleep);

	return c;
}

/********************************************
// function name: 	BankAccount::AccountNumber
// Description	: 	Returns the account's number
// Parameters	: 	None
// Returns		: 	The account's number (int)
// Exception	: 	None
// Thread-safery:	Yes
*/
int BankAccount::AccountNumber() const{
	//password and account number are only set once (at initialization) and only read (not being written to) by threads, 
	//so no lock is needed
	return m_account_number;
}

/********************************************
// function name: 	BankAccount::Password
// Description	: 	Returns the account's password
// Parameters	: 	None
// Returns		: 	The account's password (std::string)
// Exception	: 	None
// Thread-safery:	Yes
*/
string BankAccount::Password() const{
	//password and account number are only set once (at initialization) and only read (not being written to) by threads, 
	//so no lock is needed
	return m_password;
}
