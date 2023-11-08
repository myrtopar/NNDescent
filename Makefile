CC = gcc
CXX = g++
CFLAGS = -Wall
CXXFLAGS = -std=c++11 -Wall -g -Wno-unused-variable
LDFLAGS =

HEADERS = KNNGraph.hpp classes.hpp common_types.h ADTSet.h ADTMap.h

SOURCES = main.cpp classes.cpp ADTSet.c ADTMap.c 
EXECUTABLE = my_program

TEST_SOURCES = tests.cpp classes.cpp ADTSet.c ADTMap.c  
TEST_EXECUTABLE = test_program  

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@

$(TEST_EXECUTABLE): $(TEST_SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) -o $@

run: $(EXECUTABLE)
	./$(EXECUTABLE) $(ARG)

test: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

gdb: $(EXECUTABLE)
	gdb ./$(EXECUTABLE)

valgrind: $(EXECUTABLE)
	valgrind ./$(EXECUTABLE) $(ARG)

clean:
	rm -f $(EXECUTABLE) $(TEST_EXECUTABLE) 

.PHONY: all run test gdb valgrind clean
