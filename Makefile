CC = gcc
CXX = g++
CFLAGS = -Wall
CXXFLAGS = -std=c++11 -Wall -g -Wno-unused-variable
LDFLAGS =

HEADERS = KNNGraph.hpp classes.hpp common_types.h ADTSet.h
SOURCES = main.cpp classes.cpp ADTSet.c KNNGraph.cpp   
EXECUTABLE = my_program

TEST_SOURCES = tests.cpp classes.cpp ADTSet.c KNNGraph.cpp   
TEST_EXECUTABLE = test_program  

INCLUDES = -I.

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $@

$(TEST_EXECUTABLE): $(TEST_SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(TEST_SOURCES) -o $@

run: $(EXECUTABLE)
	./$(EXECUTABLE) $(ARG)

tests: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

gdb: $(EXECUTABLE)
	gdb ./$(EXECUTABLE)

valgrind: $(EXECUTABLE)
	valgrind ./$(EXECUTABLE) $(ARG)

clean:
	rm -f $(EXECUTABLE) $(TEST_EXECUTABLE)

.PHONY: all run tests gdb valgrind clean