/*
 * PageTable.cpp
 *
 *  Created on: Jun 13, 2017
 *      Author: compm
 */

#include <algorithm>
#include <cstdint>

#include "PageTable.h"
#include "VirtualMemory.h"
#include "defs.h"

//defines
#define DIR(x) (unsigned)((x & DIR_MASK) >> (OFFSET_LENGTH + TABLE_LENGTH))

using namespace std;


//********************************************
// function name: PageTable
// Description	:	Constructor
// Parameters	:	vmem - a reference to a virtual memory object
// Returns		: None
// Exception	: None
PageTable::PageTable(VirtualMemory& vmem) : m_dirs{nullptr}, mr_mem(vmem) {}


//********************************************
// function name: ~PageTable
// Description	:	Destructor
// Parameters	: None
// Returns		: None
// Exception	: None
PageTable::~PageTable() {
	for (int i = m_dirs.size() - 1; i >= 0; i--){
		if (m_dirs[i] != nullptr) delete m_dirs[i];
	}
}


//********************************************
// function name: GetPage
// Description	: Performs virtual to physical address translation
// Parameters	:	address - a virtual address to be translated
//					is_page_fault - a reference to a boolean variable, to be changed by the method. (true = page fault occured, false otherwised)
//					has_dir_been_allocated - a reference to a boolean variable, to be changed by the method (true = new dir has been created, false otherwised)
//					NOTICE: the two boolean references are required by the VirtualMemory system to keep track on the process, later outputed to the CSV log file
// Returns		: The physical address translation of the virtual address
// Exception	: None
int* PageTable::GetPage(unsigned address, bool& is_page_fault, bool& has_dir_been_allocated ) {
	//default values for the output flags
	is_page_fault = false;
	has_dir_been_allocated = false;
	
	//find the directory according to its 10 MSB bits
	auto& dir = m_dirs[DIR(address)];

	//if directory isn't allocated (first access)
	if (dir == nullptr) {
		dir = new PageDirectoryEntry;
		has_dir_been_allocated = true;
	}
	//if the dir entry isn't valid, validate it
	if (!dir->is_valid()) 
		dir->set_valid(true);
	
	//if the pte doesn't exist, create a new one
	auto pte = dir->GetPTE(address);
	if (nullptr == pte) {
		//there's a page fault
		is_page_fault = true;

		//get a free frame from the virtual memory system
		auto new_frame = mr_mem.GetFreeFrame(VPN(address));

		//insert the metadata to the FIFO swapping queue
		m_allocated_frames.push_back(make_pair(VPN(address), new_frame));
		
		//create a new page table entry
		dir->CreatePTE(address, VPN(address), new_frame);
		pte = dir->GetPTE(address);
	}
	//else page has been swapped away, a different page fault type
	else if (!pte->is_valid()) { 
		is_page_fault = true;
		//get a free frame from the pool/the device (probably change this thing. Should only load from disk. Maybe add an _is_in_disk flag)
		auto new_frame = mr_mem.GetFrameFromDisk(VPN(address));

		//insert the metadata to the FIFO swapping queue
		m_allocated_frames.push_back(make_pair(VPN(address), new_frame));
		
		//set the frame pointer
		pte->set_page_address(new_frame); 
		
		//validate the pte
		pte->set_valid(true); 
	}
	
	return pte->get_page_address();
}


//********************************************
// function name: ReleaseOldestFrame
// Description	: Performs the FIFO swapping operation
// Parameters	: None
// Returns		: a pair of VPN (unsigned)-physical frame address(int*) of the released frame
// Exception	: None
pair<unsigned, int*> PageTable::ReleaseOldestFrame()
{
	//get the oldest frame in the system
	auto oldest_allocated = m_allocated_frames.front();
	
	//remove the oldest frame in the system
	m_allocated_frames.pop_front();

	//invalidate the PTE entry that belongs to the oldest page number
	auto vbase = oldest_allocated.first << OFFSET_LENGTH;
	
	//find the directory
	auto dir_entry = m_dirs[DIR(vbase)];
	
	//get the PTE from the directory
	auto pte_entry = dir_entry->GetPTE(vbase);

	//invalidate the entry
	if (nullptr != pte_entry) pte_entry->set_valid(false);

	return oldest_allocated;
}
