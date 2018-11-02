/*
 * rwlock.cpp
 *
 *  Created on: May 24, 2017
 *      Author: dror
 */


#include "rwlock.h"

/*
 * rwlock.cpp
 *
 *  Created on: May 24, 2017
 *      Author: dror
 */

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
rwlock::rwlock(unsigned usleep_period, int num_readers, int num_writers) : 	m_sleep_period(usleep_period),
																			m_n_readers(num_readers),
																			m_n_rd_waiters(0),
																			m_max_n_readers(num_readers),
																			m_n_writers(num_writers),
																			m_n_wr_waiters(0) {
	pthread_mutex_init(&m_lock, NULL);
	pthread_cond_init(&m_read_unblock, NULL);
	pthread_cond_init(&m_write_unblock, NULL);
}


/********************************************
// function name: 	rwlock::~rwlock
// Description	: 	Destructor.
//					Destroys the mutex. 
//					The destructor doesn't use pthread_cond_destroy because helgrind throws false positive errors ('destruction of an unknown contidion variable')
// Parameters	: 	None
// Returns		: 	None
// Exception	: 	None
*/
rwlock::~rwlock() {
	//pthread_cond_destroy(&m_write_unblock); For some reason helgrind shouts "Destruction of an unknown condition variable" so we removed it
	//pthread_cond_destroy(&m_read_unblock); For some reason helgrind shouts "Destruction of an unknown condition variable" so we removed it
	pthread_mutex_destroy(&m_lock);
}


/********************************************
// function name: 	rwlock::ReadLock
// Description	: 	Acquire a shared lock
//					Waits until a unique lock is released
// Parameters	: 	None
// Returns		: 	None
// Exception	: 	None
*/
void rwlock::ReadLock() {
	pthread_mutex_lock (&m_lock);

	if(m_n_writers == 0 or m_n_wr_waiters > 0) {
		++m_n_rd_waiters;
		do pthread_cond_wait(&m_read_unblock, &m_lock);
		while(m_n_writers == 0 or m_n_wr_waiters > 0);
		--m_n_rd_waiters;
	}

	--m_n_readers;
	pthread_mutex_unlock(&m_lock);
}


/********************************************
// function name: 	rwlock::ReadUnlock
// Description	: 	Release a shared mutex from a single thread
//					In case all readers have released the lock, signal a waiting writing thread
// Parameters	: 	is_sleep - tells the lock to sleep (true) or not (false) before unlocking. (default: true)
// Returns		: 	None
// Exception	: 	None
*/
void rwlock::ReadUnlock(bool is_usleep) {
	//usleep in case told so
	if(is_usleep) usleep(m_sleep_period);


	pthread_mutex_lock (&m_lock);
	++m_n_readers;

	if (m_n_wr_waiters > 0)
		pthread_cond_broadcast(&m_write_unblock);
	else
		pthread_cond_broadcast(&m_read_unblock);

	pthread_mutex_unlock(&m_lock);

}

/********************************************
// function name: 	rwlock::WriteLock
// Description	: 	Acquire a unique lock
//					Waits until all reading threads release a shared lock or a writing thread releases a unique lock
// Parameters	: 	None
// Returns		: 	None
// Exception	: 	None
*/	
void rwlock::WriteLock() {
	pthread_mutex_lock(&m_lock);
	while (m_n_readers < m_max_n_readers or m_n_writers == 0) {
		++m_n_wr_waiters;
		do pthread_cond_wait(&m_write_unblock, &m_lock);
		while((m_n_writers == 0) or (m_n_readers < m_max_n_readers));
		//pthread_cond_wait(&no_readers, &mutex);
		--m_n_wr_waiters;
	}
	--m_n_writers;
	pthread_mutex_unlock(&m_lock);
}

/********************************************
// function name: 	rwlock::WriteLock
// Description	: 	Release a unique lock
//					In case there is writing threads waiting, signal one of them.
//					Else, in case there are reading threads waiting, signal all of them (broadcast).
// Parameters	: 	is_sleep - tells the lock to sleep (true) or not (false) before unlocking. (default: true)
// Returns		: 	None
// Exception	: 	None
*/
void rwlock::WriteUnlock(bool is_usleep) {
	//usleep in case told so
	if(is_usleep) usleep(m_sleep_period);
	//perform unlocking
	pthread_mutex_lock (&m_lock);
	++m_n_writers;
	//if there are waiting writers, signal them first
	if(m_n_wr_waiters > 0)
		pthread_cond_broadcast(&m_write_unblock);
	else if(m_n_rd_waiters  > 0) //else if there are wait readers, signal them instead
		pthread_cond_broadcast(&m_read_unblock);

	pthread_mutex_unlock(&m_lock);

}


