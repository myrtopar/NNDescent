#include <iostream>
#include "KNNGraph.hpp"

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

    const char* file_path = "datasets/00000020.bin";
    cout << "Reading Data: " << file_path << endl;

    ifstream ifs;
    ifs.open(file_path, ios::binary);
    if (!ifs.is_open()) {
        cout << "Failed to open the file." << endl;
        return 1;
    }

    Vector data = vector_create(0, NULL); 

    // Read the number of points (N)
    uint32_t N;
    ifs.read((char*)&N, sizeof(uint32_t));
    cout << "# of points: " << N << endl;

    const int num_dimensions = 100;
    for (int i = 0; i < (int)N; i++) {
        Vector point = vector_create(num_dimensions, NULL);

        for (int d = 0; d < num_dimensions; d++) {
            float value;
            ifs.read((char*)(&value), sizeof(float));
            vector_insert_last(point, &value); 
        }

        vector_insert_last(data, point); 
    }

    ifs.close();
    cout << "Finish Reading Data" << endl;
    vector_destroy(data);

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

    // KNNGraphBruteForce<MyTuple, double (*)(const MyTuple&, const MyTuple&)> myGraph(K, arraySize, myTuples, calculateEuclideanDistance);

    KNNGraph<MyTuple, double (*)(const MyTuple&, const MyTuple&)> myGraph(K, arraySize, myTuples, calculateEuclideanDistance);
    myGraph.printNeighbors();
    
    return 0;
}

