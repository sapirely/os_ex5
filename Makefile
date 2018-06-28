CC = g++
CCFLAGS= -Wextra -Wall -std=c++11 -pthread -g -DNDEBUG
TARGETS = libVirtualMemory

all: $(TARGETS)

# Library Compilation
libVirtualMemory: MemoryConstants.h VirtualMemory.h VirtualMemory.o 
	ar rcs libVirtualMemory.a VirtualMemory.o 


# Object Files
VirtualMemory.o: MemoryConstants.h PhysicalMemory.h VirtualMemory.h VirtualMemory.cpp 
	$(CC) $(CCFLAGS) -c VirtualMemory.cpp

#tar
tar:
	tar -cf ex5.tar VirtualMemory.cpp Makefile README

.PHONY: clean

clean:
	-rm -f *.o libVirtualMemory

