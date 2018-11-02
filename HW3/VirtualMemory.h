/*
 * VirtualMemory.h
 *
 *  Created on: Jun 11, 2017
 *      Author: compm
 *
 *	An implementation of a virtual memory paging system
 *	The system basic page size is 4KB, represented by the 12 LSB bits of the virtual address
 *	The system performs address translation using a page table
 */

#pragma once
#include <queue>
#include <utility>

#include "OurPointer.h"
#include "PhysMem.h"
#include "PageTable.h"
#include "SwapDevice.h"
#include "Logger.h"
#include "defs.h"

//defines
#define PAGESIZE 4096
#define VIRTMEMSIZE 4294967296
#define NUMOFFRAMES 64
#define INT_SIZE sizeof(int)
#define NEW_PAGE -1


/********************************************
// 	class name	: 	VirtualMemory
// 	Description	: 	An emulation of a virtual memory system
//
//	Members		:	allocated - size of data that has been allocated (in bytes)
//					freeFrameList - a queue of physical addresses for frames, consumed by the page table
//					pageTable - the table that utilizies the memory system
//					swDevice - a component that swaps frames to and from the "disk" (or storage system)
//					m_csv_logger - a components that writes the summary of the translations to a file, in a CSV file format

//	Methods		:	GetFreeFrame - returns a free frame to the page table
//					GetFrameFromDisk - swaps in a frame from the swap device, according to a specific page number
//					OurMalloc - allocates a chunk of virtual memory for the usage of the program
//					GetPage - returns the translation from a virtual address to a physical address according to a specific virtual address
*/
class VirtualMemory{
	friend class PageTable;
public:
	//********************************************
	// function name: VirtualMemory
	// Description	: Constructor
	// Parameters	: None
	// Returns		: None
	// Exception	: None
	VirtualMemory();
	//Initialize freeFrameList to contain all 64 frame
	//pointers as given by PhysMem Class, initialize the PageTable, give the
	//pageTable a pointer to this object so it can utilize GetFreeFrame and ReleaseFrame

	//********************************************
	// function name: ~VirtualMemory
	// Description	: Destructor
	// Parameters	: None
	// Returns		: None
	// Exception	: None
	~VirtualMemory();

public:
	//********************************************
	// function name: GetFreeFrame
	// Description	: Returns a free physical frame to the page table
	// Parameters	: new_page_num - a page number to get from the swap device, if needed (when the free list of frames is empty) 
	// Returns		: int* - the pyshical address of the frame returned to the user
	// Exception	: None
	int* GetFreeFrame(int new_page_num = NEW_PAGE);
	//Remove one item from the freeFrameList and
	//return it - suggestion, use memset(framePtr, 0, PAGESIZE) before return, might help debugging

	//********************************************
	// function name: GetFrameFromDisk
	// Description	: Returns a frame that holds that data stored in the swap device
	// Parameters	: page_num - the page number that will be swapped in from the device 
	// Returns		: int* - the pyshical address of the frame returned to the user
	// Exception	: None
	int* GetFrameFromDisk(int page_num);

	//********************************************
	// function name: ReleaseFrame
	// Description	: Releases a frame 
	// Parameters	:	page_num - the page number of the frame that will be released
	//					framePointer - the frame base physical address
	// Returns		: int* - the pyshical address of the frame returned to the user
	// Exception	: None
	void ReleaseFrame(unsigned page_num, int* framePointer); //releases the frame pointed by the framePOinter,
	//make sure you only use this function with a pointer to the beginning of the Frame! it should be the same pointer as held in PTE

	//********************************************
	// function name: OurMalloc
	// Description	: 'Allocates' a new virtual chunk of memory in the virtual memory space of the program
	// Parameters	:	size - the size of memory (in int-sized elements) to be allocated
	// Returns		: OurPointer - a new pointer with the virtual address to the new allocated memory
	// Exception	: None
	OurPointer OurMalloc(size_t size);


	//********************************************
	// function name: GetPage
	// Description	: Performs address translation using PageTable::GetPage
	// Parameters	: adr - the virtual address to be translated
	// Returns		: int* - a pointer to the base address of the physical frame
	// Exception	: None
	int* GetPage(unsigned adr);

private:
	//********************************************
	// function name: __summarize_access
	// Description	: Write summary of the access to the csv log file
	// Parameters	: None
	// Returns		: None
	// Exception	: None
	void __summarize_access();


private:
	size_t allocated = 0;
	queue<int*> freeFrameList;
	PageTable pageTable;
	SwapDevice swDevice;
	Logger m_csv_logger;


	//summary variables
	bool m_is_swapped = false;
	signed m_swapped_page = -1;
	bool m_is_page_fault = false;
	unsigned m_last_vpn = 0;
	unsigned m_last_vaddr = 0;
	int* m_last_paddr = nullptr;
	bool m_has_been_allocated = false;


};
