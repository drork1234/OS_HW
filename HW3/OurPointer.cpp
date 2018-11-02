/*
 * OurPointer.cpp
 *
 *  Created on: Jun 11, 2017
 *      Author: compm
 */

#include "defs.h"
#include "OurPointer.h"
#include "VirtualMemory.h"
#include "defs.h"
#define INC_SIZE sizeof(int)



 //********************************************
 // function name: OurPointer
 // Description	:	Constructor
 // Parameters	:	adr - the virtual address of the pointer
 //					vmem - a reference (via pointer, hence not deleted at destruction) to a virtual memory object 
 // Returns		: None
 // Exception	: None
OurPointer::OurPointer(int adr, VirtualMemory* vmem) : m_base(adr), mr_vmem(vmem){}

//********************************************
// function name: OurPointer
// Description	:	Constructor
// Parameters	:	ptr - a constant reference to an OurPointer object
// Returns		: None
// Exception	: None
OurPointer::OurPointer(OurPointer const& ptr) : m_base(ptr.m_base), mr_vmem(ptr.mr_vmem) {}

//********************************************
// function name: ~OurPointer
// Description	:	Destructor
// Parameters	: None
// Returns		: None
// Exception	: None
OurPointer::~OurPointer(){
	mr_vmem = nullptr;
	m_base = 0;
}

//********************************************
// function name: operator*
// Description	: A derefrencing operator, translates the virtual address to a physical address via the virtual memory system
// Parameters	: None
// Returns		: int& - a reference an int pointed by the physical address
// Exception	: None
int& OurPointer::operator *(){
	//ask the virtual memory system for translation from virtual address to a physical address
	int* page = mr_vmem->GetPage(m_base);
	return *(int*)((char*)page + OFFSET(m_base));
}

//********************************************
// function name: operator++
// Description	: A pre-increment operator, increases the virtual address by 4 bytes (to the 'next' int)
// Parameters	: None
// Returns		: OurPointer - a reference to this object
// Exception	: None
OurPointer& OurPointer::operator ++(){
	m_base += INC_SIZE;
	return *this;
}

//********************************************
// function name: operator++
// Description	: A post-increment operator, increases the virtual address by 4 bytes (to the 'next' int) and returns the previous address
// Parameters	: None
// Returns		: OurPointer - a new OurPointer object that holds the previous virtual address (pre-increment) of the pointer
// Exception	: None
OurPointer OurPointer::operator ++(int){
	OurPointer returned(*this);
	m_base += INC_SIZE;
	return returned;
}

//********************************************
// function name: operator--
// Description	: A pre-decrement operator, decreases the virtual address by 4 bytes
// Parameters	: None
// Returns		: OurPointer& - a reference to this object
// Exception	: None
OurPointer& OurPointer::operator --(){
	m_base -= INC_SIZE;
	return *this;
}

//********************************************
// function name: operator--
// Description	: A post-decrement operator, decreases the virtual address by 4 bytes (to the 'prev' int) and returns the previous address
// Parameters	: None
// Returns		: OurPointer - a new OurPointer object that holds the previous virtual address (pre-decrement) of the pointer
// Exception	: None
OurPointer OurPointer::operator --(int) {
	OurPointer returned(*this);
	m_base -= INC_SIZE;
	return returned;
}

//********************************************
// function name: operator=
// Description	: An assignment operator, assigns a new virtual address to the pointer
// Parameters	: ptr - a constant reference to an OurPointer reference, from which the address will be copied
// Returns		: OurPointer& - a referece to this object
// Exception	: None
OurPointer& OurPointer::operator =(OurPointer const& ptr){
	m_base = ptr.m_base;
	mr_vmem = ptr.mr_vmem;
	return *this;
}

