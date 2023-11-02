#include "acutest.h"
#include "classes.hpp"
#include "KNNGraph.hpp"

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

// test pou tha pairnei ta sets me ta neighbors kai elegxei an oi apostaseis einai ontws autes
void test_distances(void)
{
    const char *file_path = "00000020.bin";
    cout << "Reading Data: " << file_path << endl;

    ifstream ifs;
    ifs.open(file_path, ios::binary);
    if (!ifs.is_open())
    {
        cout << "Failed to open the file." << endl;
        return;
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

    DistanceFunction distanceFunction = &calculateEuclideanDistance;

    KNNDescent<float, DistanceFunction> KNNGraph(2, N, num_dimensions, data, distanceFunction);

    Vertex **array = KNNGraph.vertexArray;
    for (int i = 0; i < N; i++)
    {
        Vertex *v = array[i];
        Set nn = v->getNeighbors();
    }
}

// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
    {"test distances", test_distances},
    {NULL, NULL} // τερματίζουμε τη λίστα με NULL
};