CC = gcc
CXX = g++
CFLAGS = -Wall
CXXFLAGS = -std=c++11 -Wall -g -Wno-unused-variable
LDFLAGS =

SOURCES = main.cpp classes.cpp ADTSet.c ADTMap.c
HEADERS = KNNGraph.hpp classes.hpp common_types.h ADTSet.h ADTMap.h
EXECUTABLE = my_program

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@

run: $(EXECUTABLE)
	./$(EXECUTABLE) $(ARG)

gdb: $(EXECUTABLE)
	gdb ./$(EXECUTABLE)

valgrind: $(EXECUTABLE)
	valgrind ./$(EXECUTABLE) $(ARG)

clean:
	rm -f $(EXECUTABLE)

.PHONY: all run clean