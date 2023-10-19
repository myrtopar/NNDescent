CXX = g++
CXXFLAGS = -std=c++11 -Wall
SOURCES = KNNGraph.cpp main.cpp
HEADERS = KNNGraph.hpp
EXECUTABLE = my_program

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $(SOURCES)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

.PHONY: all run clean
