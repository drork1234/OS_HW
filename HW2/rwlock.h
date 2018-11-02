/*
 * rwlock.h
 *
 *  Created on: May 24, 2017
 *      Author: dror
 */

 /*
	Module Name : rwlock
	Description : An implementation of a read-write mechanism (rwlock) and a thread-safe counter (thread_safe_counter)
	Main methods: 	ReadLock/WriteLock - acquiring shared (Read) or unique (Write) lock on the mutex
					ReadUnlock/WriteUnlock - unlocking the lock
 */
 
#ifndef RWLOCK_H_
#define RWLOCK_H_

#include <pthread.h>
#include <limits.h>
#include <unistd.h>

/********************************************
// 	class name	: 	rwlock
// 	Description	: 	An implementation of a read-write lock mechanism
//					The lock prioritises writers over readers because of the Bank system requirments (don't want to halt execution of printing and commission charging)
//
//
//	Members		:	m_n_readers - the number of threads that currently a shared lock (read lock)
//					m_n_rd_waiters - the number of threads that currently wait for the shared lock (waiting in case a unique lock has been acquired)
//					m_max_n_readers - the maximum amount of simultanous readers allowed in the system
//					m_n_writers - the number of writers currently hold the unique lock (0 or 1)
//					m_n_wr_waiters - the number of writers currently waiting to acquire the unique lock
//					m_lock - a POSIX mutex lock
//					m_read_unblock - a POSIX condition variable. Reading threads will wait to be signaled on this variable
//					m_write_unblock - a POSIX condition variable. Writing threads will wait to be signaled on this variable
//					m_sleep_period - upon unlocking the lock, the lock will sleep the prescribed amount of time (in mili-seconds), before unlocking
			
//	Methods:		ReadLock - Acquire a shared lock
//					ReadUnlock - Release a shared lock (sleep before, if told so)
//					WriteLock - Acquire a unique lock
//					WriteUnlock - Release a unique lock (sleep before, if told so)
*/
class rwlock {
public:
	/********************************************
	// function name: 	rwlock::rwlock
	// Description	: 	Constructor.
	//					Initializes the mutex and the condition variables
	// Parameters	: 	sleep period - the amount of mili-seconds to sleep (default: 0)
	//					num_readers - maximum number of readers allowed (default : INT_MAX)
	//					num_writers - maximum number of writers allowed (default : 1)
	// Returns		: 	None
	// Exception	: 	None
	*/
	rwlock(unsigned sleep_period = 0, int num_readers = INT_MAX, int num_writers = 1);

	/********************************************
	// function name: 	rwlock::~rwlock
	// Description	: 	Destructor.
	//					Destroys the mutex. 
	//					The destructor doesn't use pthread_cond_destroy because helgrind throws false positive errors ('destruction of an unknown contidion variable')
	// Parameters	: 	None
	// Returns		: 	None
	// Exception	: 	None
	*/
	~rwlock();

public: //API
	/********************************************
	// function name: 	rwlock::ReadLock
	// Description	: 	Acquire a shared lock
	//					Waits until a unique lock is released
	// Parameters	: 	None
	// Returns		: 	None
	// Exception	: 	None
	*/
	void ReadLock();

	/********************************************
	// function name: 	rwlock::ReadUnlock
	// Description	: 	Release a shared mutex from a single thread
	//					In case all readers have released the lock, signal a waiting writing thread
	// Parameters	: 	is_sleep - tells the lock to sleep (true) or not (false) before unlocking. (default: true)
	// Returns		: 	None
	// Exception	: 	None
	*/
	void ReadUnlock(bool is_sleep = true);

	/********************************************
	// function name: 	rwlock::WriteLock
	// Description	: 	Acquire a unique lock
	//					Waits until all reading threads release a shared lock or a writing thread releases a unique lock
	// Parameters	: 	None
	// Returns		: 	None
	// Exception	: 	None
	*/
	void WriteLock();

	/********************************************
	// function name: 	rwlock::WriteLock
	// Description	: 	Release a unique lock
	//					In case there is writing threads waiting, signal one of them.
	//					Else, in case there are reading threads waiting, signal all of them (broadcast).
	// Parameters	: 	is_sleep - tells the lock to sleep (true) or not (false) before unlocking. (default: true)
	// Returns		: 	None
	// Exception	: 	None
	*/
	void WriteUnlock(bool is_sleep = true);

private:
	unsigned m_sleep_period;
	int m_n_readers;
	int m_n_rd_waiters;
	const int m_max_n_readers;
	int m_n_writers;
	int m_n_wr_waiters;
	pthread_mutex_t m_lock;
	pthread_cond_t m_read_unblock;
	pthread_cond_t m_write_unblock;

};


/********************************************
// 	class name	: 	thread_safe_counter
// 	Description	: 	an implemetation of a thread safe counter 
//
//	Members		:	m_counter - the counter of the class
//					m_top - the upper bound of the counter
//					m_lock - a read-write mechanism that makes this counter thread-safe
//					
//	Methods		:	operator++ - increases the counter by 1 (thread-safe)
					Count - returns the value of the counter
					HasReachedTop - gives an indication that the counter is equal the upper bound
					SetTop - set the counter upper bound
*/
class thread_safe_counter {
public:
	/********************************************
	// function name: 	thread_safe_counter::thread_safe_counter
	// Description	: 	Constructor
	//					Initializes the upper bound (default: INT_MAX), the counter to 0, and the lock with 0 mili-second sleeping time
	// Returns		: 	None
	// Exception	: 	None
	*/
	thread_safe_counter(int top = INT_MAX) : m_counter(0), m_top(top), m_lock(0) {

	}

	/********************************************
	// function name: 	thread_safe_counter::operator++
	// Description	: 	Increases the counter by one (thread-safe)
	// Returns		: 	thread_safe_counter& - a reference to self
	// Exception	: 	None
	*/
	thread_safe_counter& operator++(){
		m_lock.WriteLock();
		++m_counter;
		m_lock.WriteUnlock(false);

		return *this;
	}

	/********************************************
	// function name: 	thread_safe_counter::HasReachedTop
	// Description	: 	Checks if the counter has reached the upper bound (thread-safe)
	// Returns		: 	bool. true if counter is equal or greater than the upper bound, false otherwise
	// Exception	: 	None
	*/
	bool HasReachedTop() const{
		bool cond;
		m_lock.ReadLock();
		cond = (m_counter == m_top);
		m_lock.ReadUnlock(false); //do not sleep
		return cond;
	}

	/********************************************
	// function name: 	thread_safe_counter::SetTop
	// Description	: 	Sets the counter's upper bound (thread-safe)
	// Returns		: 	None
	// Exception	: 	None
	*/
	void SetTop(int top){
		m_lock.WriteLock();
		m_top = top;
		m_lock.WriteUnlock(false);//do not sleep
	}

private:
	int m_counter;
	int m_top;
	mutable rwlock m_lock;
};


#endif /* RWLOCK_H_ */
