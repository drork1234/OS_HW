/*
 * shell.cpp
 *
 *  Created on: Apr 24, 2017
 *      Author: dror
 */
#include "shell.h"
#include <utility>
#include <algorithm>
#include <cstring>
#include <errno.h>

#define RUNNING (bool)1
#define STOPPED (bool)0
#define ONESEC 	1
#define TIME_THRESH 5

#define DEBUG 1
//#undef DEBUG


//*********************Parent Handlers****************************

//********************************************
// function name: parent_handler
// Description: a signal handler for SIGINT and SIGTSTP - channeling the signals to the foreground process, if any
// Parameters: the signal value (signo)
// Returns: void
//**************************************************************************************
void parent_handler(int signo) {
	cout << endl;
	pid_t fg_proc_pid = Shell::SingInst().ForegroundPID();
	if (fg_proc_pid > 0) {
		if (signo == SIGINT) {
			kill(fg_proc_pid, SIGINT);
			return;
		}
		if (signo == SIGTSTP) {
			kill(fg_proc_pid, SIGTSTP);
			return;
		}
	}
}

//**************************************************************************************
// function name: SIGCHLD_handler
// Description: a signal handler for SIGCHLD, reaps all zombie children and unlock the
// 				shell in case the foreground process has exited/stopped
// Parameters: the signal value (signo)
// Returns: void
//**************************************************************************************
void SIGCHLD_handler(int signo) {
	signo++; //shutting up the compiler

	Shell& shl = Shell::SingInst();

	vector<pid_t> exited_jobs, stopped_jobs;
	exited_jobs.reserve(MAX_NUM_JOBS);
	stopped_jobs.reserve(MAX_NUM_JOBS);

	int status;
	pid_t pid;
	while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
		if (pid == shl.ForegroundPID()) {
			shl.SetForegroundProcess(NOJOB); //unlocking the foreground
		}

		if (WIFEXITED(status))
			exited_jobs.push_back(pid);
		else if (WIFSTOPPED(status))
			stopped_jobs.push_back(pid);
		else if(WIFSIGNALED(status)){
			//we had to add this to the program since suddenly ctrl+c doesn't respond to WIFEXITED,
			//also it did respond to in previous versions
			if(WTERMSIG(status) == 2)
				exited_jobs.push_back(pid);
		}
	}

	shl.RemoveJobs(exited_jobs);
	shl.SuspendJobs(stopped_jobs);
}


//***********************************************Search assisting functors*********************************
//**************************************************************************************
// Class name: JobCmpFn
// Description: a functor used to search a job according to it's process ID
// Members: the process ID to be looked for
//**************************************************************************************
class JobCmpFn {
public:
	JobCmpFn(int pid = 0) :
			m_pid(pid) {
	}
	bool operator()(job const& job_) {
		return job_.ProcessID() == m_pid;
	}
private:
	int m_pid;
};

//**************************************************************************************
// Class name: JobCmpPtrFn
// Description: same as JobCmpFn, except the data type in the container in which it
//				operates is a pointer type and not an object(/reference) type
// Members: the process ID to be looked for
//**************************************************************************************
class JobCmpPtrFn{
public:
	JobCmpPtrFn(pid_t pid) : m_pid(pid) {}
	bool operator()(pjob const _pjob){return _pjob->ProcessID() == m_pid;}
private:
	pid_t m_pid;
};

//*****************************************************************************************
// Class name: TaskCmpFn
// Description: a functor used to search a job according to serial number (like JobCmpFn,
//				except the comparison operation is done via task serial number and not PID)
// Members: the task id to be looked for
//*****************************************************************************************
class TaskCmpFn {
public:
	TaskCmpFn(int serial_num = 0) :
			m_serial_num(serial_num) {
	}
	bool operator()(job const& job_) {
		return job_.SerialNum() == m_serial_num;
	}
private:
	int m_serial_num;
};

//**************************************************************************************
// Class name: TaskCmpFnPtr
// Description: same as TaskCmpFn, except the data type in the container in which it
//				operates is a pointer type and not an object(/reference) type
// Members: the task serial number to be looked for
//**************************************************************************************
class TaskCmpFnPtr{
public:
	TaskCmpFnPtr(int serial_num = 0) : m_serial_num(serial_num){}
	bool operator()(pjob const job__){
		return (job__->SerialNum() == m_serial_num);
	}

private:
	int m_serial_num;
};

class CmpTasks {
public:
	bool operator () (job const& job1, job const& job2){
		return job1.SerialNum() < job2.SerialNum();
	}

private:
};
//*********************************************************************************************************


//***************************************************Shell related stuff***********************************
//******************************static member initialization**************
unsigned Shell::proc_counter = 0;

//***********************************Singleton method*********************
Shell& Shell::SingInst() {
	static Shell shell;
	return shell;
}

//***********************************Ctor methods*************************

//**************************************************************************************
// function name: Shell::Shell
// Description: same as JobCmpFn, except the data type in the container in which it
//				operates is a pointer type and not an object(/reference) type
// Arguments: the process ID to be looked for
//**************************************************************************************
Shell::Shell() : m_pid(getpid()), m_home_dir(getenv("HOME")), m_fg_activae_process(NOJOB), m_foreground_lock(false) {
	jobs.reserve(MAX_NUM_JOBS);

	//get the working directory
	char buf[MAX_LINE_SZ];
	getcwd(buf, sizeof(buf));
	m_pwd.assign(buf);

	//figure out which one is the root directory
	this->__resolve_parent_directory();

	//register the parent handler for SIGINT and SIGTSTP
	//register the signal handlers
	struct sigaction sa;
	sa.sa_flags = 0;
	sigfillset(&sa.sa_mask);

	sa.sa_handler = parent_handler;
	int signals[2] = {SIGINT, SIGTSTP};
	for (unsigned i = 0; i < 2; i++){
		if(sigaction(signals[i], &sa, NULL) == -1)
			cout << "Can't register signal handler for " << strsignal(signals[i]) << endl;
	}

	//regitser SIGCHLD handler for SIGCHLD
	sa.sa_handler = SIGCHLD_handler;
	if(sigaction(SIGCHLD, &sa, NULL) == -1){
		cout << "Can't register signal handler for SIGCHLD" << endl;
	}
	return;
}

//***********************************API methods**************************

//**************************************************************************************
// function name: Shell::AddJob
// Description: creating a new job metadata object and push it to the job container
// Arguments: the name (command line) and the process id
// Return: false if job has pID that already exists in the container, true otherwise
//**************************************************************************************
bool Shell::AddJob(const char* name, int pid) {
	//can't append more jobs if we've reached the maximum supported process count
	if (jobs.size() == MAX_NUM_JOBS)
		return false;

	/*remove this code because of slowness
	 //if trying to append a new process with the same pid, return false (and do not append a new job)
	 vector<job>::iterator job_it = find_if(jobs.begin(), jobs.end(),
	 JobCmpFn(pid));
	 if (jobs.end() != job_it)
	 return false;
	 //else
	 //add a job to the jobs container
	 * */

	//if jobs container is empty the new appended job serial number will be 1, else proc_counter + 1
	proc_counter = jobs.empty() ? 1 : (proc_counter + 1);
	jobs.push_back(job(proc_counter, name, pid));
	return true;
}

//**************************************************************************************
// function name: Shell::AddHistory
// Description: adds a command line to the history register
// Arguments: the command line string
// Return: void
//**************************************************************************************
void Shell::AddHistory(string cmdString) {
	//if the history queue is full, pop the front, then add the job to the history register
	if (history.size() == MAX_HISTORY_LENGTH) {
		history.pop_front();
	}
	history.push_back(cmdString);
}

//**************************************************************************************
// function name: Shell::PrintJobs
// Description: prints the job list to the prompt
// Arguments: None
// Return: void
//**************************************************************************************
void Shell::PrintJobs() const {
	time_t print_t; time(&print_t); //calculate the current time

	//sort the task vector using the task ID (not the pid)
	vector<job> cp_jobs(jobs);
	sort(cp_jobs.begin(), cp_jobs.end(), CmpTasks());

	for (unsigned i = 0; i < cp_jobs.size(); ++i) {
		const job& job__ = cp_jobs.at(i);
		//printf("[%d] %s : %d %lu secs/n", job__.SerialNum(), &job__.Name()[0], job__.ProcessID(), job__.StartTime());
		cout << "[" << job__.SerialNum() << "] "
				<< job__.Name() << " : "
				<< job__.ProcessID() << " "
				<< (unsigned int)difftime(print_t, job__.StartTime()) << " secs" //calculate the time differnece in seconds
				<< (job__.ExecutionStatus() ? "" : " (Stopped)")
				<< endl;
	}
	cp_jobs.clear();
}

//**************************************************************************************
// function name: Shell::PrintHistory
// Description: Prints the history register to the prompt
// Arguments: None
// Return: void
//**************************************************************************************
void Shell::PrintHistory() const {
	for (unsigned i = 0; i < history.size(); ++i)
		cout << history.at(i) << endl;
}

//**************************************************************************************
// function name: Shell::PWD
// Description: Gives the user the smash process working directory
// Arguments: None
// Return: the process absolute working directory
//**************************************************************************************
string Shell::PWD() const {
	return m_pwd;
}

//**************************************************************************************
// function name: Shell::PID
// Description: Gives the user the process ID of the smash
// Arguments: None
// Return: the process id of the smash
//**************************************************************************************
unsigned Shell::PID() const {

	return m_pid;
}

//**************************************************************************************
// function name: Shell::Chdir
// Description: Changes the working directory of the smash
// Arguments: the releative/absolute path (to be determined by the function itself) for the change
// Return: zero on success (linux chdir call succeeded), -1 otherwise
//**************************************************************************************
int Shell::Chdir(string args) {
	//if '-' is provided as an argument, change to the parent directory
	if (args == "-") {
		m_pwd = m_father_pwd;
		chdir(m_father_pwd.c_str());

		this->__resolve_parent_directory();

		return 0;
	} else {
		//if the argument passed begins with '/' concatenate it to the m_pwd, else change it regularly.
		//if the chosen dir option returns a non-zero value, print an error message
		if (((args[0] == '/') ?	chdir((m_pwd + args).c_str()) : chdir(args.c_str())) != 0) { //Sorry for this line
			fprintf(stderr, "smash error:> \"%s\" - path not found\n",
					args.c_str());
			return -1;
		} else { //directory change was successful, now update the member pwd
			char buf[MAX_LINE_SZ];
			getcwd(buf, sizeof(buf));
			m_pwd.assign(buf);

			this->__resolve_parent_directory();
			return 0;
		}
	}
}

//**************************************************************************************
// function name: Shell::Mkdir (Not required)
// Description: creates a new directory under the current pwd
// Arguments: the directory name to be created
// Return: zero on success (linux mkdir call succeeded), -1 otherwise
//**************************************************************************************
int Shell::Mkdir(string dir) const {
	if (mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		perror("Mkdir failed");
		return -1;
	}
	return 0;
}

//**************************************************************************************
// function name: Shell::Foreground
// Description: Moves a job specified by jobno to the foreground
//				- if the jobs container is empty, do nothing
//				- if the NOJOB (-1) was specified, move the last background job that was inserted to the system
//				- else, if jobno wasn't found, print an error message and return an error value
//				- else, send a SIGCONT to the job in case that it was stopped, lock the foreground
//				and wait for it to complete
// Arguments: the job number (positive int). NOJOB (-1) can also be a valid input
// Return:  NO_JOB_FOUND (2) when jobno wasn't found
//			EXIT_FAILURE (1) when failed to send a SIGCONT to a stopped
//			EXIT_SUCCESS otherwise
//**************************************************************************************
int Shell::Foreground(int jobno) {
	if (this->jobs.empty())
		return EXIT_SUCCESS;

	job _poped_job;
	if (jobno < 0) { //if job number was not specified
		//get the last background running job
		_poped_job = this->jobs.back();
	}
	else //job number was specified
	{ //search for the job using the task number (and not the pid number)
		vector<job>::iterator found_job_it = find_if(jobs.begin(), jobs.end(), TaskCmpFn(jobno));
		//if task was not found, return an error code (-1)
		if (found_job_it == jobs.end()) {
			cout << "smash error: > fg " << jobno << " - job not found" << endl;
			return JOB_NOT_FOUND;
		}

		//else
		_poped_job = *found_job_it; //extract the pointer to the task from the iterator
	}

	//set the shell forground lock
	this->SetForegroundProcess(_poped_job.ProcessID());

	cout << _poped_job.Name() << endl;




	if (_poped_job.ExecutionStatus() == STOPPED) { //send SIGCONT to the process only if it suspended
		//change the execution status of the process
		_poped_job.ToggleExecStatus(RUNNING);
		//send a SIGCONT for the pid and wait for the foreground lock to be released
		if(kill(_poped_job.ProcessID(), SIGCONT) == -1) {
			perror("smash error: > fg - failed to send SIGCONT: ");
			return EXIT_FAILURE;
		}
		cout << "smash > signal SIGCONT was sent to pid " << _poped_job.ProcessID() << endl;
	}

	//lock the screen until the foreground process completes or gets interrupted
	while (this->IsForegroundLock()) {
		pause();
	}

	return EXIT_SUCCESS;
}

//*************************************************************************************************
// function name: Shell::Background
// Description: sends a stopped job to execute in the background
//				- if the stopped process container is empty, do nothing
//				- if the NOJOB (-1) was specified, execute the last stopped job in the background
//				- else, if jobno wasn't found, print an error message and return an error value
//				- else, send a SIGCONT to the job and return
// Arguments: the job number, (NOJOB is also a valid input)
// Return: zero on success (linux mkdir call succeeded), -1 otherwise
//*************************************************************************************************
int Shell::Background(int jobno) {
	if (this->stopped_procs.empty())
		return EXIT_SUCCESS;

	job _poped_job;
	if (jobno < 0) { //if job number was not specified
		_poped_job = this->stopped_procs.front();
		stopped_procs.pop_front();
	}
	else{ 	//job number was specified
			//search for the job using the task number (and not the pid number)
		deque<job>::iterator found_job_it = find_if(stopped_procs.begin(), stopped_procs.end(), TaskCmpFn(jobno));
		//if task was not found, return an error code (-1)
		if (found_job_it == stopped_procs.end()) {
			//for debug purposes
			cerr << "smash error: > bg " << jobno << " - job wasn't found" << endl;
			return JOB_NOT_FOUND;
		}

		//else
		_poped_job = *found_job_it; //extract the pointer to the task from the iterator
		stopped_procs.erase(found_job_it);
	}

	//find the job in the main jobs container
	vector<job>::reverse_iterator found_job_iter = find_if(jobs.rbegin(), jobs.rend(), JobCmpFn(_poped_job.ProcessID()));
	if(found_job_iter == jobs.rend()){
		jobs.push_back(_poped_job);
		found_job_iter = jobs.rbegin();
	}

	//change the status of the job to RUNNING
	found_job_iter->ToggleExecStatus(RUNNING);


	//move the process to be the last process to be run in the background (the last position of the vector - back())
	rotate(jobs.rbegin(), found_job_iter, found_job_iter + 1);

	cout << _poped_job.Name() << endl;
	//continue the process and return
	if(kill(_poped_job.ProcessID(), SIGCONT) == -1){
		perror("smash error: > bg - failed to send SIGCONT");
		vector<pid_t> pids;
		pids.push_back(_poped_job.ProcessID());
		this->RemoveJobs(pids);
		return EXIT_FAILURE;
	}

	cout << "smash > signal SIGCONT was sent to pid " <<_poped_job.ProcessID() << endl;
	return EXIT_SUCCESS;
}

//**************************************************************************************
// function name: Shell::SetForegroundProcess
// Description: locks the foreground lock if pid_t is greater than zero. unlocks if NOJOB was specified
// Arguments: the process id that will run in the foreground
// Return: return true always
//**************************************************************************************
bool Shell::SetForegroundProcess(pid_t pid) {
	m_fg_activae_process = pid;
	m_foreground_lock = (pid > 0);
	return true;
}

//**************************************************************************************
// function name: Shell::IsForegroundLock
// Description: Returns an indication that the foreground lock is set
//				This method is utilized when the smash waits for a foreground process
//				to complete/be interrupted.
// Arguments: None
// Return: true if the lock is set, false otherwise
//**************************************************************************************
bool Shell::IsForegroundLock() const {
	return m_foreground_lock;
}

//**************************************************************************************
// function name: Shell::ForegroundPID
// Description: Gives the parent_handler the pid of the process that runs in the foreground
// Arguments: None
// Return: the process id the of the foreground process
//**************************************************************************************
pid_t Shell::ForegroundPID() const {
	return m_fg_activae_process;
}

//**************************************************************************************
// function name: Shell::Exit
// Description: Exits the shell, possibly killing all the current jobs spawned by the shell
// Arguments: a string that contains the argument of the command - if "kill" was specified,
//			  kill all the spawned jobs created by the shell
// Return: void
//**************************************************************************************
void Shell::Exit(string kill) {
	if (!kill.empty() && kill == "kill")
		this->__terminate_all();

	this->history.clear();
	this->jobs.clear();

	exit(EXIT_SUCCESS);
}

//**************************************************************************************
// function name: Shell::Kill
// Description: Sends a signal to a job (NOTE that if SIGKILL was sent to the jobno,
//				it will be removed from the job container, and if SIGSTOP was sent to the
//				job, the job will be suspended. This is done since SIGKILL and SIGSTOP
//				cannot be caught by user defined handlers)
// Arguments: - a string argument of the signal and a string argument of the job number
// Return: 	JOB_NOT_FOUND when no job in the job container corresponds to jobno
//			EXIT_FAILURE when linux kill failed
//			EXIT_SUCCESS otherwise
//**************************************************************************************
int Shell::Kill(const char* signal, const char* job_){
	//convert the string to int
	int jobno 		= atoi(job_);

	//convert the string to int, checking only from the character in slot No. 1 (No. 0 is '-')
	int signalno 	= atoi(&signal[1]);

	//find the job number inside the jobs container
	vector<job>::iterator found_job_iter  = find_if(jobs.begin(), jobs.end(), TaskCmpFn(jobno));

	if (found_job_iter == jobs.end()) { //if job was not found
		cerr << "smash error:> kill " << found_job_iter->SerialNum() << " - job does not exist" << endl;
		return JOB_NOT_FOUND;
	}


	if(kill(found_job_iter->ProcessID(), signalno) == -1){ //if kill failed
		cerr << "smash error:> kill " << found_job_iter->SerialNum() << " - cannot send signal" << endl;
		return EXIT_FAILURE;
	}
	cout << "smash > signal " << signal << " was sent to pid " << found_job_iter->ProcessID() << endl;

	//since SIGKILL and SIGSTOP cannot be caught by the signal handlers, we have to remove them from
	//our lists manually
	if(SIGKILL == signalno) {
		vector<pid_t> job;
		job.push_back(found_job_iter->ProcessID());
		this->RemoveJobs(job);
	}
	if(SIGSTOP == signalno){
		vector<pid_t> job;
		job.push_back(found_job_iter->ProcessID());
		this->SuspendJobs(job);
	}

	return EXIT_SUCCESS;
}

//**************************************************************************************
// function name: Shell::SuspendJobs
// Description: Pushes a list of pid's to the stopped jobs container
// Arguments: a list of pid's to be pushed to the stopped jobs container
// Return: void
//**************************************************************************************
void Shell::SuspendJobs(vector<pid_t>& pids){
	for (unsigned i = 0; i < pids.size(); ++i) { //size should be only one, the foreground pid
		vector<job>::iterator found_vjob = find_if(jobs.begin(), jobs.end(), JobCmpFn(pids[i]));
		if(jobs.end() == found_vjob) continue;

		found_vjob->ToggleExecStatus(STOPPED);
		this->stopped_procs.push_front(*found_vjob);
	}
}

//**************************************************************************************
// function name: Shell::RemoveJobs
// Description: Removes a list of jobs from the shell
// Arguments: a list of pid's to be searched and removed from jobs (and stopped jobs) container
// Return: void
//**************************************************************************************
void Shell::RemoveJobs(vector<pid_t>& pids){

	for (unsigned i = 0; i < pids.size(); ++i) {
		vector<job>::iterator found_job 	= find_if(jobs.begin(), jobs.end(), JobCmpFn(pids[i]));
		deque<job>::iterator found_pjob 	= remove_if(stopped_procs.begin(), stopped_procs.end(), JobCmpFn(pids[i]));

		//if the process counter is less or equals the serial number of the removed job,
		//decrement it
		if (proc_counter <= static_cast<unsigned>(found_job->SerialNum())){
			--proc_counter;
		}
		//remove the job metadata element from the containers
		jobs.erase(found_job);
		stopped_procs.erase(found_pjob, stopped_procs.end());
	}
}

//NOTE: SuspendJobs and RemoveJobs are utilized only in the SIGCHLD_handler

//**********************************Private methods***********************

//**************************************************************************************
// function name: Shell::__resolve_parent_directory
// Description:	finds out the parent directory of the current smash pwd
// Arguments: None
// Return: void
//**************************************************************************************
void Shell::__resolve_parent_directory() {
	char buf[MAX_LINE_SZ];
	m_father_pwd = m_pwd + "/..";
	chdir(m_father_pwd.c_str());
	getcwd(buf, sizeof(buf));
	m_father_pwd.assign(buf);
	chdir(m_pwd.c_str());
}

//**************************************************************************************
// function name: Shell::__terminate_all
// Description:	Sends SIGTERM for each active/stopped process that was spawned by the shell
//				If a process fails to terminate after 5 seconds of sending SIGTERM,
//				send a SIGKILL to the process
// Arguments: None
// Return: void
//**************************************************************************************
void Shell::__terminate_all() {
	for (unsigned i = 0; i < jobs.size(); i++) {
		job& job_ = jobs[i];
		cout << "[" << job_.SerialNum() << "] " << job_.Name() << " - " << "Sending SIGTERM... ";
		unsigned elapsed_time = 0;

		while(elapsed_time < TIME_THRESH){
			if(kill(jobs[i].ProcessID(), SIGTERM) == -1)
				perror("SIGTERM fail");

			if (errno == ESRCH) break; //process terminated regularly

			sleep(ONESEC); //else sleep for a second
			elapsed_time += ONESEC;
		}

		//if SIGTERM failed to deliver, send SIGKILL (process must terminate after SIGKILL)
		if(elapsed_time >= TIME_THRESH){
			if(kill(job_.ProcessID(), SIGKILL) == -1)
				perror("SIGKILL fail"); //can't really happen

			cout << "(5 sec passed) Sending SIGKILL... ";
		}
		cout << "Done." << endl;
	}
	return;
}
