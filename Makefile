CC = gcc
CXX = g++
CFLAGS = -Wall
CXXFLAGS = -std=c++11 -Wall -g -Wno-unused-variable
LDFLAGS =

SOURCES = main.cpp KNNGraph.cpp ADTPriorityQueue.c ADTVector.c ADTSet.c
HEADERS = KNNGraph.hpp ADTPriorityQueue.h ADTVector.h common_types.h ADTSet.h
EXECUTABLE = my_program

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@

run: $(EXECUTABLE)
	./$(EXECUTABLE)

gdb: $(EXECUTABLE)
	gdb ./$(EXECUTABLE)

valgrind: $(EXECUTABLE)
	valgrind ./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

.PHONY: all run clean