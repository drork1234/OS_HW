/*
 * ourpointer.h
 *
 *  Created on: Jun 11, 2017
 *      Author: compm
 *
 *
 *	An implmentation of OurPointer class, an emulation of an int pointer.
 *	The class holds a 32-bit virtual address (represented by an unsigned integer).
 *	The class enables all the abilities that a regular int pointer has, a.k.a assignment, incerement, decrement and derefrencing
 */
#pragma once

//forward declaration of the virtual memory class
class VirtualMemory;

/********************************************
// 	class name	: 	OurPointer
// 	Description	: 	An emulation of an int pointer 
//
//	Members		:	m_base : the virtual address that the pointer holds
//					mr_vmem : a reference to the virtual memory system, called when the pointer asks for address translation
//
//	Methods		:	operator* - a dereferencing operator
//					operator++ - an incrementing operator (post and pre-increment)
//					operator-- - a decrementing operator (post and pre-decrement)
//					operator= - an assignment operator
*/
class OurPointer {
public://RTII 

	OurPointer() = delete;

	//********************************************
	// function name: OurPointer
	// Description	:	Constructor
	// Parameters	:	adr - the virtual address of the pointer
	//					vmem - a reference (via pointer, hence not deleted at destruction) to a virtual memory object 
	// Returns		: None
	// Exception	: None
	OurPointer(int adr, VirtualMemory* vmem);

	//********************************************
	// function name: OurPointer
	// Description	:	Constructor
	// Parameters	:	ptr - a constant reference to an OurPointer object
	// Returns		: None
	// Exception	: None
	OurPointer(OurPointer const&);

	//********************************************
	// function name: ~OurPointer
	// Description	:	Destructor
	// Parameters	: None
	// Returns		: None
	// Exception	: None
	~OurPointer();

public: //API
	//********************************************
	// function name: operator=
	// Description	: An assignment operator, assigns a new virtual address to the pointer
	// Parameters	: ptr - a constant reference to an OurPointer reference, from which the address will be copied
	// Returns		: OurPointer& - a referece to this object
	// Exception	: None
	OurPointer& operator=(OurPointer const&);

	//********************************************
	// function name: operator++
	// Description	: A pre-increment operator, increases the virtual address by 4 bytes (to the 'next' int)
	// Parameters	: None
	// Returns		: OurPointer - a reference to this object
	// Exception	: None
	OurPointer& operator++(); 

	//********************************************
	// function name: operator++
	// Description	: A post-increment operator, increases the virtual address by 4 bytes (to the 'next' int) and returns the previous address
	// Parameters	: None
	// Returns		: OurPointer - a new OurPointer object that holds the previous virtual address (pre-increment) of the pointer
	// Exception	: None
	OurPointer operator++ (int);

	//********************************************
	// function name: operator--
	// Description	: A pre-decrement operator, decreases the virtual address by 4 bytes
	// Parameters	: None
	// Returns		: OurPointer& - a reference to this object
	// Exception	: None
	OurPointer& operator-- (); 

	//********************************************
	// function name: operator--
	// Description	: A post-decrement operator, decreases the virtual address by 4 bytes (to the 'prev' int) and returns the previous address
	// Parameters	: None
	// Returns		: OurPointer - a new OurPointer object that holds the previous virtual address (pre-decrement) of the pointer
	// Exception	: None
	OurPointer operator--(int); //operator--

	//********************************************
	// function name: operator*
	// Description	: A derefrencing operator, translates the virtual address to a physical address via the virtual memory system
	// Parameters	: None
	// Returns		: int& - a reference an int pointed by the physical address
	// Exception	: None
	int& operator* ();

private:
	signed m_base;
	VirtualMemory* mr_vmem; //mr = member & reference (= do not delete at destructor)
};
