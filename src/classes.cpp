#include "headers/KNNGraph.hpp"
#include "headers/classes.hpp"

using namespace std;

mutex squareMutex;

int random_int(int range, int coll)
{

    int random = (rand()) % range;
    while (random == coll)
        random = (rand()) % range;

    return random;
}

float compare_ints(Pointer a, Pointer b)
{
    return *(int *)a - *(int *)b;
}

void delete_int(void *a)
{
    delete (int *)a;
}

void delete_neighbor(void *a)
{
    delete (Neighbor *)a;
}

int *create_int(int n)
{
    int *x = new int;
    *x = n;
    return x;
}

void delete_data(float **data, uint32_t N)
{
    for (uint32_t i = 0; i < N; i++)
    {
        delete[] data[i];
        // delete[] distanceResults[i];
    }
    delete[] data;
    // delete[] distanceResults;
    delete[] squares;
}

float compare_distances(Pointer a, Pointer b)
{
    Neighbor *n1 = (Neighbor *)a;
    Neighbor *n2 = (Neighbor *)b;

    float *distance1 = n1->getDistance();
    float *distance2 = n2->getDistance();

    int *id1 = (int *)n1->getid();
    int *id2 = (int *)n2->getid();

    if (*distance1 != *distance2)
    {
        return 1000000 * (*distance1 - *distance2);
    }

    return *(int *)n1->getid() - *(int *)n2->getid();
}

Neighbor *furthest_neighbor(Set s)
{
    SetNode lastNode = set_max(s);
    if (lastNode != NULL)
        return (Neighbor *)set_node_value(s, lastNode);
    return NULL;
}

Neighbor *closest_neighbor(Set s)
{
    SetNode firstNode = set_first(s);
    if (firstNode != NULL)
        return (Neighbor *)set_node_value(s, set_first(s));
    return NULL;
}

double compare_results(int **arrayBF, int **arrayNND, int N, int K)
{
    int count = 0;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < K; j++)
        {
            int found = 0;
            for (int k = 0; k < K; k++)
            {
                if (arrayNND[i][j] == arrayBF[i][k])
                {
                    found = 1;
                    break;
                }
            }
            if (found == 0)
                count++;
        }
    }
    int number_of_edegs = N * K;
    double percent = (((double)number_of_edegs - (double)count) / (double)number_of_edegs) * 100;

    for (int i = 0; i < N; i++)
    {
        delete[] arrayNND[i];
        delete[] arrayBF[i];
    }

    delete[] arrayNND;
    delete[] arrayBF;

    return percent;
}

int contains(Neighbor *id_union[], int size, int targetId)
{
    for (int i = 0; i < size; i++)
    {
        // Check if the Neighbor object's ID matches the target ID
        if (id_union[i] == NULL)
            return 0;
        if (*(id_union[i]->getid()) == targetId)
        {
            return 1; // The ID is found in the array
        }
    }
    return 0; // The ID is not found in the array
}

float averageNeighborDistance(Set s)
{
    float total_dist = 0.0;
    for (SetNode node = set_first(s); node != SET_EOF; node = set_next(s, node))
    {
        Neighbor *n = (Neighbor *)set_node_value(s, node);
        total_dist += *(n->getDistance());
    }

    float avg_dist = total_dist / set_size(s);
    return avg_dist;
}

float averageSplitDistance(Vertex **array, int num)
{
    float total_dist = 0.0;
    for (int i = 0; i < num; i++)
    {
        Vertex *v = array[i];
    }

    // continue here!!!
    return 0.0;
}

// CLASS MEMBER DEFINITIONS
Vertex::Vertex(void *_data, int _id) : datapoint(_data), id(_id)
{
    NN = set_create(compare_distances, delete_neighbor);
    RNN = set_create(compare_distances, delete_neighbor);
    potentialNN = set_create(compare_distances, delete_neighbor);
}

int Vertex::addNeighbor(Neighbor *neighbor)
{
    if (!set_insert(NN, neighbor))
    {
        // cout << "failed to insert neighbor with id: " << *(neighbor->getid()) << " and distance " << *(neighbor->getDistance()) << " in vertex " << id << endl;
        delete neighbor;
        return 0;
    }
    return 1;
}

int Vertex::addReverseNeighbor(Neighbor *neighbor)
{
    if (!set_insert(RNN, neighbor))
    {
        delete neighbor;
        return 0;
    }
    return 1;
}

int Vertex::addPotentialNeighbor(Neighbor *neighbor)
{
    if (!set_insert(potentialNN, neighbor))
    {
        delete neighbor;
        return 0;
    }
    return 1;
}

Set Vertex::getNeighbors() const
{
    return NN;
}

Set Vertex::getReverseNeighbors() const
{
    return RNN;
}

Set Vertex::getPotentialNeighbors() const
{
    return potentialNN;
}

int Vertex::getId() const
{
    return id;
}

void Vertex::replaceNNSet(Set NewSet)
{
    set_destroy(NN);
    NN = NewSet;
}

void Vertex::resetPNNSet()
{
    set_destroy(potentialNN);
    potentialNN = set_create(compare_distances, delete_neighbor);
}

void *Vertex::getData() const
{
    return datapoint;
}

mutex &Vertex::getUpdateMutex()
{
    return updateMutex;
}

Vertex::~Vertex()
{
    set_destroy(NN);
    set_destroy(RNN);
    set_destroy(potentialNN);
}

Neighbor::Neighbor(int _id, float _distance)
{
    id = new int;
    *id = _id;
    distance = new float;
    *distance = _distance;
    flag = 1;
}

int *Neighbor::getid()
{
    return id;
}

float *Neighbor::getDistance()
{
    return distance;
}

int Neighbor::getFlag()
{
    return flag;
}

void Neighbor::setFalse()
{
    flag = 0;
}

Neighbor::~Neighbor()
{
    delete id;
    delete distance;
}

float calculateEuclideanDistance(const float *point1, const float *point2, int numDimensions)
{
    double sum = 0.0;
    for (int i = 0; i < numDimensions; i++)
    {
        double diff = point1[i] - point2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

float calculateManhattanDistance(const float *point1, const float *point2, int numDimensions)
{
    double sum = 0.0;
    for (int i = 0; i < numDimensions; i++)
    {
        sum += fabs(point1[i] - point2[i]);
    }
    return sum;
}

void parallelSquares(float **data, int start, int end, int num_dimensions) {
    std::unique_lock<std::mutex> lockupdate(squareMutex);
    for (int i = start; i < end; i++) {  
        squares[i] = dot_product(data[i], data[i], num_dimensions);
    }
}

void calculateSquares(float **data, int N, int num_dimensions)
{
    squares = new float[N];

    const int num_threads = std::thread::hardware_concurrency();
    thread threads[num_threads];

    int chunk_size = N / num_threads;
    int remaining = N % num_threads;
    int start = 0;
    int end = 0;

    for (int i = 0; i < num_threads; ++i)
    {
        end = start + chunk_size + (i < remaining ? 1 : 0);
        threads[i] = std::thread(parallelSquares, data, start, end, num_dimensions);        
        start = end;
    }

    for (int i = 0; i < num_threads; ++i) {
        threads[i].join();
    }
}

float calculateEuclideanDistance2(int i, int j, const float *point1, const float *point2, int numDimensions)
{
    float sum = 0.0;
    float xy = dot_product(point1, point2, numDimensions);
    sum = squares[i] + squares[j] - 2*xy;
    return sum;
}
