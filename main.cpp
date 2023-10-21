#include <iostream>
#include <cstdlib> 
#include "KNNGraph.hpp"

using namespace std;

#define K 2


int main() {
    
   int arraySize = 10;

    // Create an array of tuples (x, y, z)
    MyTuple myTuples[arraySize];

    for (int i = 0; i < arraySize; ++i) {
        myTuples[i].num1 = i + 1;
        myTuples[i].num2 = 2 * (i + 1);
        myTuples[i].num3 = 3 * (i + 1);
    }

    for (int i = 0; i < arraySize; ++i) {
        std::cout << "Tuple " << i << ": (" << myTuples[i].num1 << ", " << myTuples[i].num2 << ", " << myTuples[i].num3 << ")\n";
    }

    KNNGraph myGraph(K, arraySize, myTuples);
    myGraph.printNeighbors();
    

    return 0;
}

