# This is the Makefile
CXX=g++
CXXFLAGS=-g -Wall -std=c++0x
CXXLINK=$(CXX)
LIBS=
OBJS=main.o Logger.o OurPointer.o PageDirectoryEntry.o PageTable.o PageTableEntry.o PhysMem.o SwapDevice.o VirtualMemory.o
RM=rm -f

main: $(OBJS)
	$(CXXLINK) -o main $(OBJS) $(LIBS) $(CXXFLAGS)

Logger.o: Logger.cpp Logger.h
OurPointer.o: OurPointer.cpp defs.h OurPointer.h VirtualMemory.h \
 PhysMem.h PageTable.h PageDirectoryEntry.h PageTableEntry.h SwapDevice.h \
 Logger.h
PageDirectoryEntry.o: PageDirectoryEntry.cpp PageDirectoryEntry.h \
 PageTableEntry.h VirtualMemory.h OurPointer.h PhysMem.h PageTable.h \
 SwapDevice.h Logger.h defs.h
PageTable.o: PageTable.cpp PageTable.h PageDirectoryEntry.h \
 PageTableEntry.h VirtualMemory.h OurPointer.h PhysMem.h SwapDevice.h \
 Logger.h defs.h
PageTableEntry.o: PageTableEntry.cpp PageTableEntry.h
PhysMem.o: PhysMem.cpp PhysMem.h
SwapDevice.o: SwapDevice.cpp SwapDevice.h
main.o: main.cpp VirtualMemory.h OurPointer.h PhysMem.h \
 PageTable.h PageDirectoryEntry.h PageTableEntry.h SwapDevice.h Logger.h \
 defs.h
VirtualMemory.o: VirtualMemory.cpp VirtualMemory.h OurPointer.h PhysMem.h \
 PageTable.h PageDirectoryEntry.h PageTableEntry.h SwapDevice.h Logger.h \
 defs.h
defs.o: defs.h
Logger.o: Logger.h
OurPointer.o: OurPointer.h
PageDirectoryEntry.o: PageDirectoryEntry.h PageTableEntry.h
PageTableEntry.o: PageTableEntry.h
PageTable.o: PageTable.h PageDirectoryEntry.h PageTableEntry.h
PhysMem.o: PhysMem.h
SwapDevice.o: SwapDevice.h
VirtualMemory.o: VirtualMemory.h OurPointer.h PhysMem.h PageTable.h \
 PageDirectoryEntry.h PageTableEntry.h SwapDevice.h Logger.h defs.h


clean:
	$(RM) main *.o *.bak *~ "#"* core
