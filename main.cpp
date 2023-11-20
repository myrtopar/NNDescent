#include <iostream>
#include "KNNGraph.hpp"

using namespace std;

typedef double (*DistanceFunction)(const float *, const float *, int);
double calculateEuclideanDistance(const float *point1, const float *point2, int numDimensions)
{
    double sum = 0.0;
    for (int i = 0; i < numDimensions; i++)
    {
        double diff = point1[i] - point2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

typedef double (*DistanceFunction)(const float *, const float *, int);
double calculateManhattanDistance(const float *point1, const float *point2, int numDimensions)
{
    double sum = 0.0;
    for (int i = 0; i < numDimensions; i++)
    {
        sum += fabs(point1[i] - point2[i]);
    }
    return sum;
}

int main(int argc, char *argv[])
{

    if (argc != 5)
    {
        cout << "Error wrong amount of arguments.\n";
        return -1;
    }

    int K = atoi(argv[1]);
    float p = stof(argv[2]);
    int metric = atoi(argv[3]); // to encode
    char *file_path = argv[4];

    if (p > 1.0)
    {
        cout << "Error wrong sampling percent." << endl;
        return -1;
    }

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

    if (K > (int)N)
    {
        cout << "Error. Invalid K." << endl;
        delete_data(data, N);
        return -1;
    }

    DistanceFunction distanceFunction;
    if (metric == 1)
        distanceFunction = &calculateEuclideanDistance;
    else if (metric == 2)
        distanceFunction = &calculateManhattanDistance;
    else
    {
        cout << "Error. Calculate Distance Function doesn't exist.";
        return -1;
    }

    // knn descent method
    auto start1 = std::chrono::high_resolution_clock::now();
    KNNDescent<float, DistanceFunction> KNNGraph(K, N, p, num_dimensions, data, distanceFunction);
    KNNGraph.createKNNGraph();
    auto stop1 = std::chrono::high_resolution_clock::now();

    auto duration1 = std::chrono::duration_cast<std::chrono::seconds>(stop1 - start1);
    cout << "KNNDescent: " << duration1.count() << " seconds" << endl;

    // brute force method
    auto start2 = std::chrono::high_resolution_clock::now();
    KNNBruteForce<float, DistanceFunction> myGraph(K, N, num_dimensions, data, distanceFunction);
    auto stop2 = std::chrono::high_resolution_clock::now();

    auto duration2 = std::chrono::duration_cast<std::chrono::seconds>(stop2 - start2);
    cout << "Brute Force: " << duration2.count() << " seconds" << endl;

    // extract knn descent and brute force method results into a list and compare them, to find similarity percentage
    int **NND = KNNGraph.extract_neighbors_to_list();
    int **BF = myGraph.extract_neighbors_to_list();
    double percentage = compare_results(BF, NND, (int)N, K);
    if (percentage > 90.0)
    {
        cout << "\x1b[32msimilarity percentage: " << percentage << "%"
             << "\x1b[0m" << endl;
    }
    else
    {
        cout << "\x1b[31msimilarity percentage: " << percentage << "%"
             << "\x1b[0m" << endl;
    }

    // void **narray = KNNGraph.NNSinglePoint(data[10]);

    delete_data(data, N);

    return 0;
}
