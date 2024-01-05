CC = gcc
CXX = g++
CFLAGS = -Wall
CXXFLAGS = -std=c++17 -Wall -g -Wno-unused-variable -Wno-reorder -pthread 
LDFLAGS =

SRC_DIR = src
HEADERS_DIR = headers

# File paths
HEADERS = $(HEADERS_DIR)/KNNGraph.hpp $(HEADERS_DIR)/classes.hpp $(HEADERS_DIR)/common_types.h $(HEADERS_DIR)/ADTSet.h $(HEADERS_DIR)/rp_trees.hpp $(HEADERS_DIR)/KNNBruteForce.hpp
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/classes.cpp $(SRC_DIR)/ADTSet.c $(SRC_DIR)/KNNGraph.cpp $(SRC_DIR)/rp_trees.cpp $(SRC_DIR)/KNNBruteForce.cpp
EXECUTABLE = my_program

TEST_SOURCES = $(SRC_DIR)/tests.cpp $(SRC_DIR)/classes.cpp $(SRC_DIR)/ADTSet.c $(SRC_DIR)/KNNGraph.cpp $(SRC_DIR)/rp_trees.cpp $(SRC_DIR)/KNNBruteForce.cpp
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