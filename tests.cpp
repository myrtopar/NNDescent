// #include "acutest.h"
// #include "classes.hpp"
// #include "KNNGraph.hpp"

// typedef double (*DistanceFunction)(const float *, const float *, int);
// double calculateEuclideanDistance(const float *point1, const float *point2, int numDimensions)
// {

//     double sum = 0.0;
//     for (int i = 0; i < numDimensions; i++)
//     {
//         double diff = point1[i] - point2[i];
//         sum += diff * diff;
//     }
//     return sqrt(sum);
// }

// // test pou tha pairnei ta sets me ta neighbors kai elegxei an oi apostaseis einai ontws autes
// void test_distances(void)
// {
//     const char *file_path = "datasets/00000020.bin";
//     cout << "Reading Data: " << file_path << endl;

//     ifstream ifs;
//     ifs.open(file_path, ios::binary);
//     if (!ifs.is_open())
//     {
//         cout << "Failed to open the file." << endl;
//         return;
//     }

//     // Read the number of points (N)
//     uint32_t N;
//     ifs.read((char *)&N, sizeof(uint32_t));
//     cout << "# of points: " << N << endl;

//     const int num_dimensions = 100;

//     // Create arrays for storing the data
//     float **data = new float *[N];
//     for (uint32_t i = 0; i < N; i++)
//     {
//         data[i] = new float[num_dimensions];
//     }

//     for (uint32_t i = 0; i < N; i++)
//     {
//         for (int d = 0; d < num_dimensions; d++)
//         {
//             float value;
//             ifs.read((char *)(&value), sizeof(float));
//             data[i][d] = value;
//         }
//     }

//     ifs.close();

//     DistanceFunction distanceFunction = &calculateEuclideanDistance;

//     KNNDescent<float, DistanceFunction> KNNGraph(10, N, num_dimensions, data, distanceFunction);
//     KNNGraph.createKNNGraph();

//     Vertex **array = KNNGraph.vertexArray;
//     for (uint32_t i = 0; i < N; i++)
//     {
//         Vertex *v = array[i];
//         Set nn = v->getNeighbors();
//     }
// }

// // Λίστα με όλα τα tests προς εκτέλεση
// TEST_LIST = {
//     {"test distances", test_distances},
//     {NULL, NULL} // τερματίζουμε τη λίστα με NULL
// };



#include "acutest.h"
#include "classes.hpp"
#include "KNNGraph.hpp"


// Define your test data and variables globally
const char *file_path = "datasets/00000050.bin";
uint32_t N;
const int num_dimensions = 100;
float **data;
int K;

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


void start_program() {
    cout << "\nReading Data: " << file_path << endl;

    ifstream ifs;
    ifs.open(file_path, ios::binary);
    if (!ifs.is_open())
    {
        cout << "Failed to open the file." << endl;
        TEST_ASSERT(0);
        return;
    }

    // Read the number of points (N) and store it in the global variable
    ifs.read((char *)&N, sizeof(uint32_t));
    cout << "# of points: " << N << endl;

    const int num_dimensions = 100;

    // Create arrays for storing the data
    data = new float *[N];
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

}


void end_program(void)
{
    // Clean up data arrays
    for (uint32_t i = 0; i < N; i++)
    {
        delete[] data[i];
    }
    delete[] data;
}


// test pou tha pairnei ta sets me ta neighbors kai elegxei an oi apostaseis einai ontws autes
void test_distances(void)
{
    start_program();
    DistanceFunction distanceFunction = &calculateEuclideanDistance;

    KNNDescent<float, DistanceFunction> KNNGraph(10, N, num_dimensions, data, distanceFunction);

    Vertex **array = KNNGraph.vertexArray;

    for (int r = 0; r < 10; r++) {
        cout << " \n\n";
        KNNGraph.calculatePotentialNewNeighbors();
        

        // array to store distances for each node
        double **nodeDistancesNN = new double *[N];
        double **nodeDistancesRN = new double *[N];
        double **nodeDistancesPN = new double *[N];

        for (uint32_t i = 0; i < N; i++)
        {
            cout << "Node no " << i << " --> ";
            Vertex *v = array[i];
            Set nn = v->getNeighbors();
            Set rn = v->getReverseNeighbors();
            Set pn = v->getPotentialNeighbors();

            nodeDistancesNN[i] = new double[set_size(nn)];
            nodeDistancesRN[i] = new double[set_size(rn)];
            nodeDistancesPN[i] = new double[set_size(pn)];

            int j = 0;
            for (SetNode node = set_first(nn); node != SET_EOF; node = set_next(nn, node))
            {
                Neighbor *n = (Neighbor *)set_node_value(nn, node);
                int *id = n->getid();
                double *dist = n->getDistance();
                double exactDistance = calculateEuclideanDistance(data[i], data[*id], num_dimensions);
                nodeDistancesNN[i][j] = *dist;
                cout << "\e[1;32m" << *id << "\e[0m"
                    << "(" << *dist << " = " << exactDistance << "), ";
                TEST_ASSERT((exactDistance - *dist) == 0);
                j++;
            }

            j = 0;
            for (SetNode node = set_first(rn); node != SET_EOF; node = set_next(rn, node))
            {
                Neighbor *n = (Neighbor *)set_node_value(rn, node);
                int *id = n->getid();
                double *dist = n->getDistance();
                double exactDistance = calculateEuclideanDistance(data[i], data[*id], num_dimensions);
                nodeDistancesRN[i][j] = *dist;
                cout << "\e[1;32m" << *id << "\e[0m"
                    << "(" << *dist << "), ";
                TEST_ASSERT((exactDistance - *dist) == 0);
                j++;
            }

            j = 0;
            for (SetNode node = set_first(pn); node != SET_EOF; node = set_next(pn, node))
            {
                Neighbor *n = (Neighbor *)set_node_value(pn, node);
                int *id = n->getid();
                double *dist = n->getDistance();
                double exactDistance = calculateEuclideanDistance(data[i], data[*id], num_dimensions);
                nodeDistancesPN[i][j] = *dist;
                cout << "\e[1;32m" << *id << "\e[0m"
                    << "(" << *dist << "), ";
                TEST_ASSERT((exactDistance - *dist) == 0);
                j++;
            }
            cout << endl;
        }

        if (KNNGraph.updateGraph() == 0)
            break;

    }
    
   
}




void test_potential() {
    start_program();
    
    DistanceFunction distanceFunction = &calculateEuclideanDistance;

    KNNDescent<float, DistanceFunction> KNNGraph(10, N, num_dimensions, data, distanceFunction);

    for (int i = 0; i < 10; i++) {
        KNNGraph.calculatePotentialNewNeighbors();
        if (KNNGraph.updateGraph() == 0)
            break;

        for(uint32_t j = 0; j < N; j++) {
            Vertex *vertex = KNNGraph.vertexArray[j];
            TEST_ASSERT((set_size(vertex->getPotentialNeighbors()) == 0));
        }
    }

    end_program();
}



// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
    {"test distances", test_distances},
    {"test_a", test_potential},
    {NULL, NULL} // τερματίζουμε τη λίστα με NULL
};