#include <iostream>
#include "KNNGraph.hpp"

using namespace std;

#define K 2

// a simple euclidean distance calculator for 3D tuples
// user can pass any other distance calctulator function
// double calculateEuclideanDistance(const MyTuple& point1, const MyTuple& point2) {
//     double dx = point2.num1 - point1.num1;
//     double dy = point2.num2 - point1.num2;
//     double dz = point2.num3 - point1.num3;

//     // cout << "dx, dy, dz = " << dx << " " << dy << " " << dz << " \n";
//     return std::sqrt(dx * dx + dy * dy + dz * dz);
// }

typedef double (*DistanceFunction)(const float *, const float *, int);
double calculateEuclideanDistance(const float *point1, const float *point2, int numDimensions)
{
    // cout << "\nPoint1: \n";
    // for (int i = 0; i < numDimensions; i++) {
    //     cout << point1[i] << " ";
    // }
    // cout << "\nPoint2: \n";
    // for (int i = 0; i < numDimensions; i++) {
    //     cout << point2[i] << " ";
    // }
    double sum = 0.0;
    for (int i = 0; i < numDimensions; i++)
    {
        double diff = point1[i] - point2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

int main()
{

    const char *file_path = "00000020.bin";
    cout << "Reading Data: " << file_path << endl;

    ifstream ifs;
    ifs.open(file_path, ios::binary);
    if (!ifs.is_open())
    {
        cout << "Failed to open the file." << endl;
        return 1;
    }

    // Read the number of points (N)
    uint32_t N;
    ifs.read((char *)&N, sizeof(uint32_t));
    cout << "# of points: " << N << endl;

    const int num_dimensions = 100;

    // Create arrays for storing the data
    float **data = new float *[N];
    for (uint32_t i = 0; i < N; i++)
    {
        data[i] = new float[num_dimensions];
    }

    for (uint32_t i = 0; i < N; i++)
    {
        for (int d = 0; d < num_dimensions; d++)
        {
            float value;
            ifs.read((char *)(&value), sizeof(float));
            data[i][d] = value;
        }
    }

    ifs.close();
    cout << "Finish Reading Data" << endl;

    // print data
    // for (uint32_t i = 0; i < N; i++)
    // {
    //     for (int d = 0; d < num_dimensions; d++)
    //     {
    //         cout << data[i][d] << ", ";
    //     }
    //     cout << "\n\n";
    // }

    DistanceFunction distanceFunction = &calculateEuclideanDistance;

    cout << "BRUTE FORCE GRAPH" << endl;
    KNNGraphBruteForce<float, DistanceFunction> myGraph(K, N, num_dimensions, data, distanceFunction);

    // int arraySize = 10;

    // // Create an array of tuples (x, y, z)
    // MyTuple myTuples[arraySize];

    // for (int i = 0; i < arraySize; ++i) {
    //     myTuples[i].num1 = i + 1;
    //     myTuples[i].num2 = 2 * (i + 1);
    //     myTuples[i].num3 = 3 * (i + 1);
    // }

    // for (int i = 0; i < arraySize; ++i) {
    //     std::cout << "Tuple " << i << ": (" << myTuples[i].num1 << ", " << myTuples[i].num2 << ", " << myTuples[i].num3 << ")\n";
    // }

    // KNNGraphBruteForce<MyTuple, double (*)(const MyTuple&, const MyTuple&)> myGraph(K, arraySize, 2, myTuples, calculateEuclideanDistance);
    // KNNGraph<MyTuple, double (*)(const MyTuple&, const MyTuple&)> myGraph(K, arraySize, myTuples, calculateEuclideanDistance);
    // myGraph.printNeighbors();

    cout << endl;
    cout << "KNN DESCENT GRAPH" << endl;
    KNNGraph<float, DistanceFunction> myGraph2(K, N, num_dimensions, data, distanceFunction);

    myGraph2.printNeighbors();
    myGraph2.printReverseNeighbors();

    delete_data(data, N);

    return 0;
}
