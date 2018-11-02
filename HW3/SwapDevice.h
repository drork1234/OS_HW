#include <unordered_map>
#include <vector>
#include <string.h>
#include <iostream>

//defines
#define PAGESIZE 4096

/********************************************
// 	class name	: 	SwapDevice
// 	Description	: 	An implementation of a swap device component
//
//	Members		:	_size - number of frames allocated
//					_data - an associative container that translates VPN to page frame address that holds the data of the page
//
//	Methods		:	WriteFrameToSwapDevice - Writes an entire 4KB frame to the swap device
//					ReadFrameFromSwapDevice - Reads an entire 4KB frame from the swap device
*/
class SwapDevice{
	public://Memory
		//********************************************
		// function name: SwapDevice
		// Description	: Constructor
		// Parameters	: None
		// Returns		: None
		// Exception	: None
		SwapDevice();

		//********************************************
		// function name: SwapDevice
		// Description	: Destructor. Releases all the memory held by the device
		// Parameters	: None
		// Returns		: None
		// Exception	: None
		~SwapDevice();

	public: //API
		//********************************************
		// function name: WriteFrameToSwapDevice
		// Description	: Write an entire 4KB page to the swap device
		// Parameters	:	pageNumber - the number of the page to be written to the device
		//					pageOut - the base address of the 4KB page
		// Returns		: None
		// Exception	: None
        void WriteFrameToSwapDevice(int pageNumber, int* pageOut); //Write the content of page to the swap device, "pageOut" is the frame base pointer where the page is now allocated
       
		//********************************************
		// function name: ReadFrameFromSwapDevice
		// Description	: Write an entire 4KB page to the swap device
		// Parameters	:	pageNumber - the number of the page to be read from the device
		//					pageOut - the base address of the 4KB page
		// Returns		: int - 0 when reading succeeded (swapped page has been found according to it's page number), -1 otherwise
		// Exception	: None
		int ReadFrameFromSwapDevice(int pageNumber, int* pageIn); //Put the content of the page in "page". " pageIn " is the frame base pointer to where the page is about to be allocated, returns -1 if page is not stored in the swap device.
    private:
        size_t _size;
        std::unordered_map<int, int*> _data;

};
