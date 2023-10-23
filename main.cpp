#include <iostream>
#include <cstdlib> 
#include <cmath>
#include "KNNGraph.hpp"
#include <string>

using namespace std;

#define K 2


// a simple euclidean distance calculator for 3D tuples
// user can pass any other distance calctulator function
double calculateEuclideanDistance(const MyTuple& point1, const MyTuple& point2) {
    double dx = point2.num1 - point1.num1;
    double dy = point2.num2 - point1.num2;
    double dz = point2.num3 - point1.num3;

    // cout << "dx, dy, dz = " << dx << " " << dy << " " << dz << " \n";
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}


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

    KNNGraph<MyTuple, double (*)(const MyTuple&, const MyTuple&)> myGraph(K, arraySize, myTuples, calculateEuclideanDistance);
    myGraph.printNeighbors();
    // myGraph.calculateKNN();
    
    return 0;
}

