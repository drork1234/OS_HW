/*
 * shell.h
 *
 *  Created on: Apr 24, 2017
 *      Author: dror
 */

#ifndef SHELL_H_
#define SHELL_H_

//STL headers
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stack>

//linux headers
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define MAX_NUM_JOBS 100
#define MAX_HISTORY_LENGTH 50
#define MAX_LINE_SZ 80

//our defines
#define JOB_NOT_FOUND 	2
#define INVALID_JOB_REF 3
#define NOJOB -1

using namespace std;

//**************************************************Our Classes*********************************************************************
//class representing a job's metadata
class job {
public: //ctor's
	job() : m_serial_num(-1), m_name(), m_pid(-1), m_start_time(), m_exec_status(false){}
	job(signed serial_num, const char* name, int pid) : 	m_serial_num(serial_num),
															m_name(name),
															m_pid(pid),
															m_exec_status(true){time(&m_start_time);}
	//cp ctor
	job(const job& cjob) : 	m_serial_num(cjob.m_serial_num),
							m_name(cjob.m_name),
							m_pid(cjob.m_pid),
							m_start_time(cjob.m_start_time),
							m_exec_status(cjob.m_exec_status){}

public: //API
	signed SerialNum() 		const { return m_serial_num; }
	string Name() 			const { return m_name; }
	int ProcessID() 		const { return m_pid; }
	time_t StartTime() 		const { return m_start_time; }
	bool ExecutionStatus() 	const {	return m_exec_status;}
	void ToggleExecStatus(bool status){
		m_exec_status = status;
		return;
	}
	job& operator=(job const& job__){
		m_serial_num = job__.m_serial_num;
		m_name = job__.m_name;
		m_pid = job__.m_pid;
		m_start_time = job__.m_start_time;
		m_exec_status = job__.m_exec_status;

		return *this;
	}

private:
	signed m_serial_num;
	string m_name;
	int m_pid;
	time_t m_start_time;
	bool m_exec_status; //true - running, false - stopped
};
typedef job* pjob;




//class handling the history and jobs of the shell
class Shell {
public:
	//singleton method
	static Shell& SingInst();

public: //API
	bool AddJob(const char* name, int pid);

	void AddHistory(string cmdString);

	void PrintJobs() const ;

	void PrintHistory() const;

	string PWD() const;

	unsigned PID() const;

	int Chdir(string args);

	int Mkdir(string dir) const;

	int Foreground(int jobno);

	int Background(int jobno);

	void Exit(string kill);

	bool SetForegroundProcess(pid_t pid);

	bool IsForegroundLock() const ;

	pid_t ForegroundPID() const;

	void SuspendJobs(vector<pid_t>& pids);

	void RemoveJobs(vector<pid_t>& pids);

	int Kill(const char* signal, const char* job);

private:
	Shell();

private:
	void __resolve_parent_directory();

	void __terminate_all();

private:
	vector<job> jobs;
	deque<string> history;
	deque<job> stopped_procs; //a stack of references (not dynamically allocated pointers) to hold the stopped jobs. can't use std::ref (no C++11 allowed), hence using pointers
	unsigned m_pid;
	string m_pwd, m_father_pwd, m_home_dir;
	pid_t m_fg_activae_process;
	bool m_foreground_lock;
	static unsigned proc_counter;
};


//**************************************************Our Classes*********************************************************************




#endif /* SHELL_H_ */
