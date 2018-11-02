#include "PhysMem.h"

int* PhysMem::physMem;


//********************************************
// function name: Access
// Description	: Utilize the singleton. Calls the private constructor for the first time it is called 
// Parameters	: None
// Returns		: PhysMem& - a reference to this singleton object 
// Exception	: None
PhysMem & PhysMem::Access() {
	static PhysMem single;
	//only the first time it's called, the memory is allocated
	if (physMem == NULL) {
		physMem = (int*)malloc(PHYSMEMSZ);
	}
	return single;
}


//********************************************
// function name: GetFrame
// Description	: Returns a frame according to a specific frame number
// Parameters	: frameNumber - number of the frame to be requested by this method
// Returns		: int* - a frame base address
// Exception	: None
int* PhysMem::GetFrame(int frameNumber) {
	//the system supports only 64 frames of size 4KB, comprising a 256KB physical memory space
	if (frameNumber < 0 || frameNumber >= 64)
		throw "Invalid Frame Number";
	return &(physMem[1024 * frameNumber]);
}


//********************************************
// function name: ~PhysMem
// Description	: Denstructor (non accessable), releases the memory held by this object
// Parameters	: None
// Returns		: None
// Exception	: None
PhysMem::~PhysMem() {
	//release the memory
	if (physMem){
		free(physMem);
		physMem = nullptr;
	}
}