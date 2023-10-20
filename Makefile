CC = gcc
CXX = g++
CFLAGS = -Wall
CXXFLAGS = -std=c++11 -Wall
LDFLAGS =

# Define the source files and the executable name
SOURCES = main.cpp KNNGraph.cpp ADTMap.c
HEADERS = KNNGraph.hpp ADTMap.h common_types.h
EXECUTABLE = my_program

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

.PHONY: all run clean





# CXX = g++
# CXXFLAGS = -std=c++11 -Wall
# SOURCES = KNNGraph.cpp main.cpp
# HEADERS = KNNGraph.hpp
# EXECUTABLE = my_program

# all: $(EXECUTABLE)

# $(EXECUTABLE): $(SOURCES)
# 	$(CXX) $(CXXFLAGS) -o $@ $(SOURCES)

# run: $(EXECUTABLE)
# 	./$(EXECUTABLE)

# clean:
# 	rm -f $(EXECUTABLE)

# .PHONY: all run clean
