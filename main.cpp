#include <iostream>
#include "KNNGraph.hpp"

using namespace std;

#define K 2

// a simple euclidean distance calculator for 3D tuples
// user can pass any other distance calctulator function
double calculateEuclideanDistance(const MyTuple &point1, const MyTuple &point2)
{
    double dx = point2.num1 - point1.num1;
    double dy = point2.num2 - point1.num2;
    double dz = point2.num3 - point1.num3;

    // cout << "dx, dy, dz = " << dx << " " << dy << " " << dz << " \n";
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

int *create_int(int value)
{
    int *p = new int;
    *p = value;
    return p;
}

int main()
{

    // const char* file_path = "datasets/00000020.bin";
    // cout << "Reading Data: " << file_path << endl;

    // ifstream ifs;
    // ifs.open(file_path, ios::binary);
    // if (!ifs.is_open()) {
    //     cout << "Failed to open the file." << endl;
    //     return 1;
    // }

    // Vector data = vector_create(0, NULL);

    // // Read the number of points (N)
    // uint32_t N;
    // ifs.read((char*)&N, sizeof(uint32_t));
    // cout << "# of points: " << N << endl;

    // const int num_dimensions = 100;
    // for (int i = 0; i < (int)N; i++) {
    //     Vector point = vector_create(num_dimensions, NULL);

    //     for (int d = 0; d < num_dimensions; d++) {
    //         float value;
    //         ifs.read((char*)(&value), sizeof(float));
    //         vector_insert_last(point, &value);
    //     }

    //     vector_insert_last(data, point);
    // }

    // ifs.close();
    // cout << "Finish Reading Data" << endl;
    // vector_destroy(data);

    int arraySize = 10;

    // Create an array of tuples (x, y, z)
    MyTuple myTuples[arraySize];

    for (int i = 0; i < arraySize; ++i)
    {
        myTuples[i].num1 = i + 1;
        myTuples[i].num2 = 2 * (i + 1);
        myTuples[i].num3 = 3 * (i + 1);
    }

    for (int i = 0; i < arraySize; ++i)
    {
        std::cout << "Tuple " << i << ": (" << myTuples[i].num1 << ", " << myTuples[i].num2 << ", " << myTuples[i].num3 << ")\n";
    }

    // KNNGraphBruteForce<MyTuple, double (*)(const MyTuple&, const MyTuple&)> myGraph(K, arraySize, myTuples, calculateEuclideanDistance);

    KNNGraph<MyTuple, double (*)(const MyTuple &, const MyTuple &)> myGraph(K, arraySize, myTuples, calculateEuclideanDistance);
    myGraph.printNeighbors();
    myGraph.calculateKNN();
    myGraph.printPotentialNeighbors();

    return 0;
}

// Set myset = set_create(compare_ints, NULL);
// int *n1 = create_int(100);
// int *n2 = create_int(150);
// int *n3 = create_int(200);
// int *n4 = create_int(250);
// int *n5 = create_int(300);
// int *n6 = create_int(150);

// set_insert(myset, n1);
// set_insert(myset, n2);
// set_insert(myset, n3);
// set_insert(myset, n4);
// set_insert(myset, n5);

// void **array = set_to_array(myset);
// for (int i = 0; i < set_size(myset); i++)
// {
//     int *n = (int *)array[i];
//     cout << *n << " ";
// }

// cout << endl;
// set_insert(myset, n6);

// array = set_to_array(myset);
// for (int i = 0; i < set_size(myset); i++)
// {
//     int *n = (int *)array[i];
//     cout << *n << " ";
// }

// cout << endl;
