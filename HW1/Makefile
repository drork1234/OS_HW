# This is the Makefile
CXX=g++
CXXFLAGS=-g -Wall -std=c++0x
CXXLINK=$(CXX)
LIBS=
OBJS=smash.o commands.o shell.o signals.o
RM=rm -f

smash: $(OBJS)
	$(CXXLINK) -o smash $(OBJS) $(LIBS)

commands.o: commands.cpp commands.h shell.h signals.h
commands.o: commands.h
shell.o: shell.cpp shell.h
shell.o: shell.h
signals.o: signals.cpp signals.h
signals.o: signals.h
smash.o: smash.cpp commands.h signals.h

clean:
	$(RM) smash *.o *.bak *~ "#"* core
