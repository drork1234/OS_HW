# This is the Makefile
CXX=g++
CXXFLAGS=-g -Wall -std=c++0x -pthread
CXXLINK=$(CXX)
LIBS=
OBJS=main.o BankAccount.o rwlock.o Bank.o ATM.o ATM_manager.o Logger.o System.o
RM=rm -f

Bank: $(OBJS)
	$(CXXLINK) -o Bank $(OBJS) $(LIBS) $(CXXFLAGS)

ATM.o: ATM.cpp ATM.h Bank.h BankAccount.h rwlock.h Logger.h
ATM.o: ATM.h Bank.h BankAccount.h rwlock.h Logger.h
ATM_manager.o: ATM_manager.cpp ATM_manager.h ATM.h Bank.h BankAccount.h \
 rwlock.h Logger.h
ATM_manager.o: ATM_manager.h ATM.h Bank.h BankAccount.h rwlock.h Logger.h
BankAccount.o: BankAccount.cpp BankAccount.h rwlock.h
BankAccount.o: BankAccount.h rwlock.h
Bank.o: Bank.cpp Bank.h BankAccount.h rwlock.h Logger.h
Bank.o: Bank.h BankAccount.h rwlock.h Logger.h
Logger.o: Logger.cpp Logger.h rwlock.h
Logger.o: Logger.h rwlock.h
main.o: main.cpp System.h Bank.h BankAccount.h rwlock.h Logger.h \
 ATM_manager.h ATM.h
rwlock.o: rwlock.cpp rwlock.h
rwlock.o: rwlock.h
System.o: System.cpp System.h Bank.h BankAccount.h rwlock.h Logger.h \
 ATM_manager.h ATM.h
System.o: System.h Bank.h BankAccount.h rwlock.h Logger.h ATM_manager.h \
 ATM.h


clean:
	$(RM) Bank *.o *.bak *~ "#"* core
