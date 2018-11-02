/*
 * PageDirectoryEntry.h
 *
 *  Created on: Jun 11, 2017
 *      Author: compm
 *
 *	An implmentation of a page table directory (DIR)
 *	The class holds an array of 1024 page table entries (PTE), and represents the first level of translation of the virtual address
 *	
 */

#pragma once
#include <array>
#include "PageTableEntry.h"

//defins
#define NUM_PTE 1024

using namespace std;

//typedefs
using pPageTableEntry = PageTableEntry*;

//forward declaration
class VirtualMemory; 

/********************************************
// 	class name	: 	PageDirectoryEntry
// 	Description	: 	An implementation of a page table directory
//
//	Members		:	m_PTEs - an array that holds 1024 PageTableEntries 
//
//	Methods		:	GetPTE - returns a page table entry, according to a virtual address
//					CreatePTE - creates a new page table entry
*/
class PageDirectoryEntry : public PageTableEntry{
public:
	//********************************************
	// function name: PageDirectoryEntry
	// Description	:	Constructor
	// Parameters	:	is_valid - sets the 'valid' flag of the DIR (default - false)
	// Returns		: None
	// Exception	: None
	PageDirectoryEntry(bool is_valid = false);

	//********************************************
	// function name: ~PageDirectoryEntry
	// Description	: Destructor
	// Parameters	: None
	// Returns		: None
	// Exception	: None
	~PageDirectoryEntry();

public:
	//********************************************
	// function name: GetPTE
	// Description	: Returns a page table entry, according to a virtual address
	// Parameters	: address - the virtual address according to which the entry will be searched (using the 10 intermidiate bits of the address)
	// Returns		: pPageTableEntry - a pointer to the page table entry
	// Exception	: None
	pPageTableEntry GetPTE(unsigned address);

	//********************************************
	// function name: CreatePTE
	// Description	: Creates a new page table entry, according to a specific virtual address
	// Parameters	:	vaddress - the virtual address according to which the entry will be created (the table will be inserted in place according to the [21:12] bits of the virtual address)
	//					page_num - the page number of the frame that the PTE will be pointing to
	//					paddress - the actual physical address of the frame 
	// Returns		: None
	// Exception	: None
	void CreatePTE(unsigned vaddress, unsigned page_num, int* paddress);

private:
	array<pPageTableEntry, NUM_PTE> m_PTEs;
};
