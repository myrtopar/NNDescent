#include <iostream>
#include "KNNGraph.hpp"

using namespace std;

#define K 3

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

    const char *file_path = "00000050.bin";
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

    // cout << endl;
    cout << "KNN DESCENT GRAPH" << endl;

    KNNDescent<float, DistanceFunction> KNNGraph(K, N, num_dimensions, data, distanceFunction);

    // KNNGraph.printNeighbors();
    // KNNGraph.printReverseNeighbors();
    // KNNGraph.calculatePotentialNewNeighbors();
    KNNGraph.createKNNGraph();

    cout << "BRUTE FORCE GRAPH" << endl;
    KNNBruteForce<float, DistanceFunction> myGraph(K, N, num_dimensions, data, distanceFunction);

    delete_data(data, N);

    return 0;
}
