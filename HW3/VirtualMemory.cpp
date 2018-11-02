#include "VirtualMemory.h"


//********************************************
// function name: VirtualMemory
// Description	: Constructor
// Parameters	: None
// Returns		: None
// Exception	: None
VirtualMemory::VirtualMemory() : pageTable(*this), swDevice(), m_csv_logger("log.csv") {
	PhysMem& phMem = PhysMem::Access();
	//get all frames of the physical memory to the free list of frames, starting at frame 0
	for (unsigned i = 0; i < NUMOFFRAMES; i++)
		freeFrameList.push(phMem.GetFrame(i));

}


//********************************************
// function name: ~VirtualMemory
// Description	: Destructor
// Parameters	: None
// Returns		: None
// Exception	: None
VirtualMemory::~VirtualMemory() {}

//********************************************
// function name: GetFreeFrame
// Description	: Returns a free physical frame to the page table
// Parameters	: new_page_num - a page number to get from the swap device, if needed (when the free list of frames is empty) 
// Returns		: int* - the pyshical address of the frame returned to the user
// Exception	: None
int* VirtualMemory::GetFreeFrame(int new_page_num) {
	int* frame = nullptr;

	if (freeFrameList.empty()) { //if the free frame list is empty, that means that the physical memory is full
		auto oldest_table_frame = pageTable.ReleaseOldestFrame();
		//save this information to later logging
		m_swapped_page = oldest_table_frame.first;

		//write the swapped page to the disk
		swDevice.WriteFrameToSwapDevice(oldest_table_frame.first, oldest_table_frame.second);

		//read the contents of the new page from the disk
		swDevice.ReadFrameFromSwapDevice(new_page_num, oldest_table_frame.second);
		frame = oldest_table_frame.second;
	}
	else {
		m_swapped_page = -1;
		frame = freeFrameList.front();
		freeFrameList.pop();
	}

	m_is_swapped = (m_swapped_page >= 0);
	memset(frame, 0, PAGESIZE);
	return frame;
}


//********************************************
// function name: GetFrameFromDisk
// Description	: Returns a frame that holds that data stored in the swap device
// Parameters	: page_num - the page number that will be swapped in from the device 
// Returns		: int* - the pyshical address of the frame returned to the user
// Exception	: None
int* VirtualMemory::GetFrameFromDisk(int page_num) { //for use only when a pte already exists but has been swapped away
	int* frame = nullptr;
	if (freeFrameList.empty()) { //if memory is full, swap the oldest page to the disk
		auto oldest_table_frame = pageTable.ReleaseOldestFrame();
		//save this information to later logging
		m_swapped_page = oldest_table_frame.first;
		//write the swapped data to the disk
		swDevice.WriteFrameToSwapDevice(oldest_table_frame.first, oldest_table_frame.second);
		frame = oldest_table_frame.second;
	}
	else { //if not, take a free frame
		m_swapped_page = -1;
		frame = freeFrameList.front();
		freeFrameList.pop();
	}

	//now, copy the data from the disk
	swDevice.ReadFrameFromSwapDevice(page_num, frame);

	m_is_swapped = (m_swapped_page >= 0);
	return frame;
}


//********************************************
// function name: ReleaseFrame
// Description	: Releases a frame 
// Parameters	:	page_num - the page number of the frame that will be released
//					framePointer - the frame base physical address
// Returns		: int* - the pyshical address of the frame returned to the user
// Exception	: None
void VirtualMemory::ReleaseFrame(unsigned page_num, int* framePointer) {
	freeFrameList.push(framePointer); //pushes a frame to the free frame list ('Releases' a frame)
}


//********************************************
// function name: OurMalloc
// Description	: 'Allocates' a new virtual chunk of memory in the virtual memory space of the program
// Parameters	:	size - the size of memory (in int-sized elements) to be allocated
// Returns		: OurPointer - a new pointer with the virtual address to the new allocated memory
// Exception	: None
OurPointer VirtualMemory::OurMalloc(size_t size) {
	if (allocated + 4 * size >= (VIRTMEMSIZE >> 2)) {
		throw "We are limited to 4294967296 bytes with our 32 bit address size";
	}
	//else
	OurPointer ptr(allocated, this);
	allocated += INT_SIZE * size;
	return ptr;
}


//********************************************
// function name: GetPage
// Description	: Performs address translation using PageTable::GetPage
// Parameters	: adr - the virtual address to be translated
// Returns		: int* - a pointer to the base address of the physical frame
// Exception	: None
int* VirtualMemory::GetPage(unsigned adr) {
	//reset all flags
	m_swapped_page = -1;
	m_is_swapped = false;

	//set the values
	m_last_vaddr = adr;
	m_last_vpn = VPN(adr);

	//perform the translation
	auto paddr = pageTable.GetPage(adr, m_is_page_fault, m_has_been_allocated);
	m_last_paddr = (int*)((char*)paddr + OFFSET(adr));
	
	//write to the csv log file
	__summarize_access();
	return paddr;
}


//********************************************
// function name: __summarize_access
// Description	: Write summary of the access to the csv log file
// Parameters	: None
// Returns		: None
// Exception	: None
void VirtualMemory::__summarize_access() {
	m_csv_logger << m_last_vpn << "," << m_last_vaddr << "," << (char*)m_last_paddr - (char*)PhysMem::Access().GetFrame(0) << ","
		<< m_is_page_fault << "," << m_is_swapped << "," << m_swapped_page << "," << m_has_been_allocated << endl;
}