#include "KNNGraph.hpp"
#include "classes.hpp"

using namespace std;

double compare_ints(Pointer a, Pointer b)
{
    return *(int *)a - *(int *)b;
}

void delete_int(void *a)
{
    delete (int *)a;
}

<<<<<<< HEAD
void delete_double(void *a)
{
    delete (double *)a;
}

=======
>>>>>>> ebdf46bbcb35040e5ef0c96143b4685392b4a7c8
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

<<<<<<< HEAD
double *create_double(double n)
{
    double *x = new double;
    *x = n;
    return x;
}

=======
>>>>>>> ebdf46bbcb35040e5ef0c96143b4685392b4a7c8
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

    if (*distance1 != *distance2)
        return 1000000 * (*distance1 - *distance2);

    return *(int *)n1->getid() - *(int *)n2->getid();
}

Neighbor *furthest_neighbor(Set s)
{
    SetNode lastNode = set_last(s);
    if (lastNode != NULL)
        return (Neighbor *)set_node_value(s, set_last(s));
    return NULL;
}

Neighbor *closest_neighbor(Set s)
{
    SetNode firstNode = set_first(s);
    if (firstNode != NULL)
        return (Neighbor *)set_node_value(s, set_first(s));
    return NULL;
}

void compare_results(int **arrayBF, int **arrayNND, int N, int K)
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
    cout << "\x1b[32msimilarity percentage: " << percent << "%"
         << "\x1b[0m" << endl;

    for (int i = 0; i < N; i++)
    {
        delete[] arrayNND[i];
        delete[] arrayBF[i];
    }

    delete[] arrayNND;
    delete[] arrayBF;
}

Vertex::Vertex(void *_data) : datapoint(_data)
{
    NN = set_create(compare_distances, delete_neighbor);
    RNN = set_create(compare_distances, delete_neighbor);
    potentialNN = set_create(compare_distances, delete_neighbor);
<<<<<<< HEAD
    distances = map_create(compare_ints, delete_int, delete_double);
    map_set_hash_function(distances, hash_string);
=======
>>>>>>> ebdf46bbcb35040e5ef0c96143b4685392b4a7c8
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
    set_insert(potentialNN, neighbor);
}

<<<<<<< HEAD
void Vertex::addDistance(int id, double dist)
{
    int *nid = create_int(id);
    double *ndist = create_double(dist);
    map_insert(distances, nid, ndist);
}

=======
>>>>>>> ebdf46bbcb35040e5ef0c96143b4685392b4a7c8
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

<<<<<<< HEAD
Map Vertex::getDistances() const
{
    return distances;
}

=======
>>>>>>> ebdf46bbcb35040e5ef0c96143b4685392b4a7c8
void Vertex::replaceNNSet(Set NewSet)
{
    set_destroy(NN);
    NN = NewSet;
}

<<<<<<< HEAD
void Vertex::replaceRNNSet(Set NewSet)
{
    set_destroy(RNN);
    RNN = NewSet;
}

=======
>>>>>>> ebdf46bbcb35040e5ef0c96143b4685392b4a7c8
void Vertex::resetPNNSet()
{
    set_destroy(potentialNN);
    potentialNN = set_create(compare_distances, delete_neighbor);
}

<<<<<<< HEAD
void Vertex::resetRNNSet()
{
    set_destroy(RNN);
    RNN = set_create(compare_distances, delete_neighbor);
}

=======
>>>>>>> ebdf46bbcb35040e5ef0c96143b4685392b4a7c8
void *Vertex::getData() const
{
    return datapoint;
}

Vertex::~Vertex()
{
    set_destroy(NN);
    set_destroy(RNN);
    set_destroy(potentialNN);
<<<<<<< HEAD
    map_destroy(distances);
=======
>>>>>>> ebdf46bbcb35040e5ef0c96143b4685392b4a7c8
}

Neighbor::Neighbor(int _id, double _distance)
{
    id = new int;
    *id = _id;
    distance = new double;
    *distance = _distance;
}

int *Neighbor::getid()
{
    return id;
}

double *Neighbor::getDistance()
{
    return distance;
}

Neighbor::~Neighbor()
{
    delete id;
    delete distance;
}
