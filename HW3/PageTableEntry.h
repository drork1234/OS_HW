/*
* PageTableEntry.h
*
*  Created on: Jun 11, 2017
*      Author: compm
*
*	An implmentation of a page table entry
*	The class holds a the physical address of the frame, the page number, and a flag that tells if the entry is valid
*
*/

#pragma once


/********************************************
// 	class name	: 	PageTableEntry
// 	Description	: 	An implementation of a page table entry
//
//	Members		:	m_base - the physical base address of the frame
//					m_page_number - the page frame number 
//					m_is_valid - a flag that indicates that the entry is valid
//
//	Methods		:	get_page_address - returns the frame physical address
//					get_page_number - the page number of the frame
//					set_page_address - sets the page frame physical address
//					is_valid - indicates that the entry is valid
//					set_valid - changes the entry 'valid' state
*/
class PageTableEntry {
public://Memory and RAII
	//********************************************
	// function name: PageTableEntry
	// Description	: Constructor
	// Parameters	:	base - the base physical address of the frame
	//					page_number - the page number of the frame
	//					valid - a flag that indicates that the entry is valid
	// Returns		: None
	// Exception	: None
	PageTableEntry(int* base = nullptr, unsigned page_number = 0, bool valid = false);

	//********************************************
	// function name: ~PageTableEntry
	// Description	: Destructor
	// Parameters	: None
	// Returns		: None
	// Exception	: None
	~PageTableEntry();

public://API
	//********************************************
	// function name: get_page_address
	// Description	: returns the physical frame address
	// Parameters	: None
	// Returns		: int* - the base physical address of the frame
	// Exception	: None
	int* get_page_address() const; 

	//********************************************
	// function name: get_page_number
	// Description	: returns the page number of the frame
	// Parameters	: None
	// Returns		: unsigned - the page number of the frame
	// Exception	: None
	unsigned get_page_number() const;

	//********************************************
	// function name: set_page_address
	// Description	: sets the physical base address of the frame
	// Parameters	: adr - the new base physical address of the frame
	// Returns		: None
	// Exception	: None
	void set_page_address(int* adr);

	//********************************************
	// function name: is_valid
	// Description	: indicates that the entry is valid
	// Parameters	: None
	// Returns		: bool - true if the entry is valid, aflse otherwise
	// Exception	: None
	bool is_valid() const;

	//********************************************
	// function name: set_valid
	// Description	: changes the state of the 'valid' flag
	// Parameters	: valid - the new status of the 'valid' flag of the entry
	// Returns		: None
	// Exception	: None
	void set_valid(bool valid); //Allows to set whether the entry is valid
	
protected:
	int* m_base = nullptr;
	unsigned m_page_number = 0;
	bool m_is_valid = false;
	
};
