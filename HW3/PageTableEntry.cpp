/*
 * PageTableEntry.cpp
 *
 *  Created on: Jun 11, 2017
 *      Author: compm
 */

#include "PageTableEntry.h"

 //********************************************
 // function name: PageTableEntry
 // Description	: Constructor
 // Parameters	:	base - the base physical address of the frame
 //					page_number - the page number of the frame
 //					valid - a flag that indicates that the entry is valid
 // Returns		: None
 // Exception	: None
PageTableEntry::PageTableEntry(int* base, unsigned page_number, bool valid) : m_base(base), m_page_number(page_number), m_is_valid(valid){}


//********************************************
// function name: ~PageTableEntry
// Description	: Destructor
// Parameters	: None
// Returns		: None
// Exception	: None
PageTableEntry::~PageTableEntry() {
	m_is_valid = false;
	m_base = nullptr;
}


//********************************************
// function name: get_page_address
// Description	: returns the physical frame address
// Parameters	: None
// Returns		: int* - the base physical address of the frame
// Exception	: None
int* PageTableEntry::get_page_address() const {return m_base;}


//********************************************
// function name: get_page_number
// Description	: returns the page number of the frame
// Parameters	: None
// Returns		: unsigned - the page number of the frame
// Exception	: None
unsigned PageTableEntry::get_page_number() const { return m_page_number; }


//********************************************
// function name: set_page_address
// Description	: sets the physical base address of the frame
// Parameters	: adr - the new base physical address of the frame
// Returns		: None
// Exception	: None
void PageTableEntry::set_page_address(int* adr) { m_base = adr;}


//********************************************
// function name: is_valid
// Description	: indicates that the entry is valid
// Parameters	: None
// Returns		: bool - true if the entry is valid, aflse otherwise
// Exception	: None
bool PageTableEntry::is_valid() const {return m_is_valid;}


//********************************************
// function name: set_valid
// Description	: changes the state of the 'valid' flag
// Parameters	: valid - the new status of the 'valid' flag of the entry
// Returns		: None
// Exception	: None
void PageTableEntry::set_valid(bool valid) {m_is_valid = valid;}
