#include "headers/KNNGraph.hpp"
#include "headers/classes.hpp"

using namespace std;

double compare_ints(Pointer a, Pointer b)
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
    }
    delete[] data;
}

double compare_distances(Pointer a, Pointer b)
{
    Neighbor *n1 = (Neighbor *)a;
    Neighbor *n2 = (Neighbor *)b;

    double *distance1 = n1->getDistance();
    double *distance2 = n2->getDistance();

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

// CLASS MEMBER DEFINITIONS
Vertex::Vertex(void *_data) : datapoint(_data)
{
    NN = set_create(compare_distances, delete_neighbor);
    RNN = set_create(compare_distances, delete_neighbor);
    potentialNN = set_create(compare_distances, delete_neighbor);
}

void Vertex::addNeighbor(Neighbor *neighbor)
{
    set_insert(NN, neighbor);
}

void Vertex::addReverseNeighbor(Neighbor *neighbor)
{
    set_insert(RNN, neighbor);
}

void Vertex::addPotentialNeighbor(Neighbor *neighbor)
{
    if (!set_insert(potentialNN, neighbor))
        delete neighbor;
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

Vertex::~Vertex()
{
    set_destroy(NN);
    set_destroy(RNN);
    set_destroy(potentialNN);
}

Neighbor::Neighbor(int _id, double _distance)
{
    id = new int;
    *id = _id;
    distance = new double;
    *distance = _distance;
    flag = 1;
}

int *Neighbor::getid()
{
    return id;
}

double *Neighbor::getDistance()
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