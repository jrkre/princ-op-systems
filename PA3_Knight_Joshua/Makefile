#This is variant of a boilerplate Makefile I've used for years to compile C projects for class pls dont kill me

CXX = gcc
CFLAGS = -Wall -g -pthread

TARGET = sched #exec
HEADERS =  #include
SRCS = sched.c #src

OBJECTS := $(patsubst %.cpp,%.o,$(SRCS))

#Rule that states that default all and clean are make commands and not associated with any files
.PHONY: default all clean

default: all

#Rule that defers make all to the TARGET rule
all: $(TARGET)

#Rule to compile a single object file
%.o: %.cpp $(HEADERS)
	$(CXX) $(CFLAGS) -c $< -o $@

#Rule that makes all object files in the OBJECTS list, then links them all together to produce TARGET executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) $(LIBS) -o $@

#Rule to clean up the build (removes iteratively all object files .o and the execitable TARGET)
clean:
	-rm -f *.o
	-rm -f $(TARGET)