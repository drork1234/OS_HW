/*
 * PageTable.h
 *
 *  Created on: Jun 11, 2017
 *      Author: compm
 *
 *	An implmentation of a 2 level page table
 *	The class holds an array of 1024 page table directories (DIR), each holding an array of 1024 page table entries (PTE), each PTE points 
 *	to a 4KB physical memory slice. Total memory covered by a single PageTable is 1024*1024*4096 = 2**32 bytes = 4GB, which enables the process to use the entire
 *	memory space.
 *
 */

#pragma once

#include "PageDirectoryEntry.h"
#include <array>
#include <deque>
#include <utility>

//defines
#define NUM_DIRECTORIES 1024

using namespace std;

//typedefs
using pPageDirectoryEntry = PageDirectoryEntry*;

//forward declaration
class VirtualMemory;

/********************************************
// 	class name	: 	PageDirectoryEntry
// 	Description	: 	An implementation of a page table directory
//					The class is non-copyable and non-assignable
//
//	Members		:	m_dirs - an array that holds 1024 page table directories
//					mr_mem - a reference to a virtual memory object
//					m_allocated_frames - a queue that holds data about the order of the allocated frames, supporting an implementation
//										of a FIFO frame-swapping mechanism
//
//	Methods		:	GetPage - returns a physical address according to a virtual address 
//					ReleaseOldestFrame - a FIFO page-swapping mechanism. Releases the oldest frame allocated in the system, invalidates it's PTE
*/
class PageTable {
public:
	PageTable(PageTable const&) = delete;
	PageTable& operator = (PageTable const&) = delete;

	//********************************************
	// function name: PageTable
	// Description	:	Constructor
	// Parameters	:	vmem - a reference to a virtual memory object
	// Returns		: None
	// Exception	: None
	PageTable(VirtualMemory& vmem);
	
	//********************************************
	// function name: ~PageTable
	// Description	:	Destructor
	// Parameters	: None
	// Returns		: None
	// Exception	: None
	~PageTable();

public:
	//********************************************
	// function name: GetPage
	// Description	: Performs virtual to physical address translation
	// Parameters	:	address - a virtual address to be translated
	//					is_page_fault - a reference to a boolean variable, to be changed by the method. (true = page fault occured, false otherwised)
	//					has_dir_been_allocated - a reference to a boolean variable, to be changed by the method (true = new dir has been created, false otherwised)
	//					NOTICE: the two boolean references are required by the VirtualMemory system to keep track on the process, later outputed to the CSV log file
	// Returns		: The physical address translation of the virtual address
	// Exception	: None
	int* GetPage(unsigned address, bool& is_page_fault, bool& has_dir_been_allocated);

	//********************************************
	// function name: ReleaseOldestFrame
	// Description	: Performs the FIFO swapping operation
	// Parameters	: None
	// Returns		: a pair of VPN (unsigned)-physical frame address(int*) of the released frame
	// Exception	: None
	pair<unsigned, int*> ReleaseOldestFrame();

private:
	array<pPageDirectoryEntry, NUM_DIRECTORIES> m_dirs;
	VirtualMemory& mr_mem;
	deque<pair<unsigned, int*>> m_allocated_frames;
};
