/*
 * PageDirectoryEntry.cpp
 *
 *  Created on: Jun 12, 2017
 *      Author: compm
 */


#include "PageDirectoryEntry.h"
#include "VirtualMemory.h"
#include "defs.h"
#include <vector>

#define PTE(x) (unsigned)((x & TABLE_MASK) >> OFFSET_LENGTH)

 //********************************************
 // function name: PageDirectoryEntry
 // Description	:	Constructor
 // Parameters	:	is_valid - sets the 'valid' flag of the DIR (default - false)
 // Returns		: None
 // Exception	: None
PageDirectoryEntry::PageDirectoryEntry(bool is_valid) : PageTableEntry(nullptr, is_valid), m_PTEs{nullptr}{
	//initialize all the page table entries to NULL (pointing to nothing)
}


//********************************************
// function name: ~PageDirectoryEntry
// Description	: Destructor
// Parameters	: None
// Returns		: None
// Exception	: None
PageDirectoryEntry::~PageDirectoryEntry() {
	//delete all the page table entries
	for (int i = m_PTEs.size() - 1; i >= 0; i--){
		if (m_PTEs[i] != nullptr) delete m_PTEs[i];
	}
}


//********************************************
// function name: GetPTE
// Description	: Returns a page table entry, according to a virtual address
// Parameters	: address - the virtual address according to which the entry will be searched (using the 10 intermidiate bits of the address)
// Returns		: pPageTableEntry - a pointer to the page table entry
// Exception	: None
pPageTableEntry PageDirectoryEntry::GetPTE(unsigned address) {
	//this virtual address is masked using only the 12:21 bits of the vaddress value, then using these bits to access the page
	//directory and return the corresponding PTE
	return m_PTEs[PTE(address)];
}


//********************************************
// function name: CreatePTE
// Description	: Creates a new page table entry, according to a specific virtual address
// Parameters	:	vaddress - the virtual address according to which the entry will be created (the table will be inserted in place according to the [21:12] bits of the virtual address)
//					page_num - the page number of the frame that the PTE will be pointing to
//					paddress - the actual physical address of the frame 
// Returns		: None
// Exception	: None
void PageDirectoryEntry::CreatePTE(unsigned vaddress, unsigned page_num, int * paddress){
	//the page directory searches for the correct PTE according to the virtual address
	//then checks if the PTE is not NULL. If it's not NULL, delete the PTE (it's dynamically allocated).
	//then create a new PTE
	auto& pte = m_PTEs[PTE(vaddress)];
	if (nullptr != pte) delete pte;
	pte = new PageTableEntry(paddress, page_num, true);
}
