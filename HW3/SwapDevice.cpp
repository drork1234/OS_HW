#include "SwapDevice.h"
#include <cstdlib>
#define FRAMESZ 4096

using namespace std;


//********************************************
// function name: SwapDevice
// Description	: Constructor
// Parameters	: None
// Returns		: None
// Exception	: None
SwapDevice::SwapDevice() : _size(0), _data() {
}


//********************************************
// function name: SwapDevice
// Description	: Destructor. Releases all the memory held by the device
// Parameters	: None
// Returns		: None
// Exception	: None
SwapDevice::~SwapDevice()
{
	//release all the memory held by the device
	for (auto& data : _data){
		if (nullptr != data.second)
			free(data.second);
	}
}


//********************************************
// function name: WriteFrameToSwapDevice
// Description	: Write an entire 4KB page to the swap device
// Parameters	:	pageNumber - the number of the page to be written to the device
//					pageOut - the base address of the 4KB page
// Returns		: None
// Exception	: None
void SwapDevice::WriteFrameToSwapDevice(int pageNumber, int* pageOut) {
	//if the page number doesn't represent any frame, allocate memory for it, then copy the data from the page to there
    if (_data[pageNumber] == NULL) {
        if (++_size > 104857) {
            cerr << "The swap device seems to be using too much space, worth investigating" << endl;
        }
        _data[pageNumber] = (int*)malloc(FRAMESZ);
    }
    memcpy(_data[pageNumber], pageOut, FRAMESZ);
}


//********************************************
// function name: ReadFrameFromSwapDevice
// Description	: Write an entire 4KB page to the swap device
// Parameters	:	pageNumber - the number of the page to be read from the device
//					pageOut - the base address of the 4KB page
// Returns		: int - 0 when reading succeeded (swapped page has been found according to it's page number), -1 otherwise
// Exception	: None
int SwapDevice::ReadFrameFromSwapDevice(int pageNumber, int* pageIn) {
	//if there's no page number corresponding to a valid frame, return -1, else read the data from the device and return 0
    if (_data[pageNumber] == NULL) {
        return -1;
    }
    memcpy(pageIn, _data[pageNumber], FRAMESZ);
    return 0;
}
