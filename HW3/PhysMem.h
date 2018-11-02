/*
* PhysMem.h
*
*
*	An emulation of a physical memory (RAM) in a virtual memory system
*	The class holds a memory chunk of 256KB in size, representing the total physical memory available in the system
*	The class is implemented as a singleton
*
*/

#include <stdlib.h>
#include <iostream>

using namespace std;

//defines
#define PHYSMEMSZ 262144

/********************************************
// 	class name	: 	PhysMem
// 	Description	: 	An emulation of a physical memory pool
//					The class is a non-copyable, non assignable singleton 
//
//	Members		:	physMem - a static memory pool, representing a single contiguous physical memory

//	Methods		:	GetFrame - returns a frame according to a specific frame number
//					Access - statis method, returns a reference to the singleton object
*/
class PhysMem {
friend class VirtualMemory;
	public:
	//********************************************
	// function name: Access
	// Description	: Utilize the singleton. Calls the private constructor for the first time it is called 
	// Parameters	: None
	// Returns		: PhysMem& - a reference to this singleton object 
	// Exception	: None
	static PhysMem& Access();

	private:
		//********************************************
		// function name: PhysMem
		// Description	: Constructor (non accessable)
		// Parameters	: None
		// Returns		: None
		// Exception	: None
		PhysMem() {}

		//********************************************
		// function name: ~PhysMem
		// Description	: Denstructor (non accessable), releases the memory held by this object
		// Parameters	: None
		// Returns		: None
		// Exception	: None
		~PhysMem();
		static int* physMem;

	public:
		PhysMem(PhysMem const&) = delete;
		void operator=(PhysMem const&) = delete;

		//********************************************
		// function name: GetFrame
		// Description	: Returns a frame according to a specific frame number
		// Parameters	: frameNumber - number of the frame to be requested by this method
		// Returns		: int* - a frame base address
		// Exception	: None
		int* GetFrame(int frameNumber);
};
