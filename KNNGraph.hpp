#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include "ADTSet.h"
#include "classes.hpp"
#include <sstream>
#include <fstream>

using namespace std;

// due to the template usage, the implementation of the functions below should be available in this file

template <typename DataType, typename DistanceFunction>
class KNNDescent
{
private:
    int K;
    int size;
    float sampling;
    int dimensions;
    DistanceFunction distance;

public:
    KNNDescent(int _K, int _size, float _sampling, int dimensions, DataType **myTuples, DistanceFunction _distance);

    void createRandomGraph(int K, Vertex **vertexArray);
    void calculatePotentialNewNeighbors();
    int updateGraph();
    void createKNNGraph();
    int **extract_neighbors_to_list();
    void **NNSinglePoint(void *data);

    ~KNNDescent();

    Vertex **vertexArray;
};

template <typename DataType, typename DistanceFunction>
class KNNBruteForce
{
private:
    Vertex **vertexArray;
    int K;
    int size;
    int dimensions;
    DistanceFunction distance;

public:
    KNNBruteForce(int _K, int _size, int dimensions, DataType **data, DistanceFunction _distance);

    void calculateKNNBF() const;
    int **extract_neighbors_to_list();

    ~KNNBruteForce();
};

template <typename DataType, typename DistanceFunction>
KNNBruteForce<DataType, DistanceFunction>::KNNBruteForce(int _K, int _size, int _dimensions, DataType **data, DistanceFunction _metricFunction) : K(_K), size(_size), dimensions(_dimensions), distance(_metricFunction)
{
    vertexArray = new Vertex *[size];
    for (int i = 0; i < size; i++)
    {
        vertexArray[i] = new Vertex(data[i]);
    }
    calculateKNNBF();
}

template <typename DataType, typename DistanceFunction>
void KNNBruteForce<DataType, DistanceFunction>::calculateKNNBF() const
{
    cout << "\nConstructing a graph of " << size << " elements, looking for " << K << " nearest neighbors" << endl;

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];

        for (int j = 0; j < size; j++)
        {
            if (i == j)
                continue;

            Vertex *nvertex = vertexArray[j];

            DataType *vertexData = static_cast<DataType *>(vertex->getData());
            DataType *neighborData = static_cast<DataType *>(nvertex->getData());

            double dist = distance(vertexData, neighborData, dimensions);

            Neighbor *newNeighbor = new Neighbor(j, dist);
            vertex->addNeighbor(newNeighbor);
        }
    }

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        Set allNodes = vertex->getNeighbors();
        Set Kneighbors = set_create(compare_distances, delete_neighbor);

        int j = 0;
        for (SetNode node = set_first(allNodes); node != SET_EOF; node = set_next(allNodes, node))
        {
            if (j == K)
                break;
            Neighbor *p = (Neighbor *)set_node_value(allNodes, node);
            Neighbor *newNeighbor = new Neighbor(*p->getid(), *p->getDistance());
            set_insert(Kneighbors, newNeighbor);
            j++;
        }
        vertex->replaceNNSet(Kneighbors);
    }
}

template <typename DataType, typename DistanceFunction>
int **KNNBruteForce<DataType, DistanceFunction>::extract_neighbors_to_list()
{
    int **neighbors = new int *[size];
    for (int i = 0; i < size; i++)
    {
        neighbors[i] = new int[K];
    } // row col

    for (int i = 0; i < size; i++)
    {
        Vertex *v = vertexArray[i];
        Set nn = v->getNeighbors();

        int j = 0;
        for (SetNode node = set_first(nn); node != SET_EOF; node = set_next(nn, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(nn, node);
            neighbors[i][j] = *n->getid();
            j++;
        }
    }

    return neighbors;
}

template <typename DataType, typename DistanceFunction>
KNNBruteForce<DataType, DistanceFunction>::~KNNBruteForce()
{
    for (int i = 0; i < size; i++)
    {
        delete vertexArray[i];
    }
    delete[] vertexArray;
}

////////////////////////////////// KNNDESCENT //////////////////////////////////
template <typename DataType, typename DistanceFunction>
KNNDescent<DataType, DistanceFunction>::KNNDescent(int _K, int _size, float _sampling, int _dimensions, DataType **data, DistanceFunction _metricFunction) : K(_K), size(_size), sampling(_sampling), dimensions(_dimensions), distance(_metricFunction)
{
    cout << "\nConstructing a graph of " << size << " elements, looking for " << K << " nearest neighbors" << endl;
    vertexArray = new Vertex *[size];
    for (int i = 0; i < size; i++)
    {
        vertexArray[i] = new Vertex(data[i]);
    }
    createRandomGraph(K, vertexArray);
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::createRandomGraph(int K, Vertex **vertexArray)
{
    // Connect each vertex with K random neighbors
    for (int i = 0; i < size; i++)
    {
        Set usedIds = set_create(compare_ints, delete_int);

        for (int j = 0; j < K; j++)
        {
            int randomNeighborIndex;
            do
            {
                randomNeighborIndex = rand() % size;
            } while (randomNeighborIndex == i || (set_find(usedIds, (void *)&randomNeighborIndex) != NULL));
            // if the random generated index is the vertex itself, we find another random neighbor
            // if the random generated index has already been chosen as one of the k random nearest neighbors previously, we find another random neighbor

            int *randomId = create_int(randomNeighborIndex);
            set_insert(usedIds, randomId);

            // calculate distance
            DataType *vertexData = static_cast<DataType *>(vertexArray[i]->getData());
            DataType *neighborData = static_cast<DataType *>(vertexArray[randomNeighborIndex]->getData());

            double dist = distance(vertexData, neighborData, dimensions);

            Neighbor *newNeighbor = new Neighbor(randomNeighborIndex, dist);
            vertexArray[i]->addNeighbor(newNeighbor);

            Neighbor *newReverseNeighbor = new Neighbor(i, dist);
            vertexArray[randomNeighborIndex]->addReverseNeighbor(newReverseNeighbor);
        }
        set_destroy(usedIds);
    }
}

template <typename DataType, typename DistanceFunction>
KNNDescent<DataType, DistanceFunction>::~KNNDescent()
{
    for (int i = 0; i < size; i++)
    {
        delete vertexArray[i]; // Delete each Vertex
    }
    delete[] vertexArray; // Delete the array of Vertex pointers
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::calculatePotentialNewNeighbors()
{
    // Every calculatePotentialNewNeighbors call calculates the potential neighbors of each vector and prepares the graph for updates.

    // for every vertex, we find its potential neighbors for update
    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        // cout << "Vertex " << i << endl;

        Set neighbors = vertex->getNeighbors();
        Set reverseNeighbors = vertex->getReverseNeighbors();

        // UNION
        Set id_union = set_create(compare_distances, NULL);
        for (SetNode node = set_first(neighbors); node != SET_EOF; node = set_next(neighbors, node)) {
            Neighbor *n = (Neighbor *)set_node_value(neighbors, node);
            set_insert(id_union, n);
        }
        for (SetNode node = set_first(reverseNeighbors); node != SET_EOF; node = set_next(reverseNeighbors, node)) {
            Neighbor *rn = (Neighbor *)set_node_value(reverseNeighbors, node);
            set_insert(id_union, rn);
        }

        // LOCAL JOIN in sets neighborArray and ReverseNeighborArray
        int j = 0;
        for (SetNode node1 = set_first(id_union); node1 != SET_EOF; node1 = set_next(id_union, node1)) {
            for (SetNode node2 = set_first(id_union); node2 != SET_EOF; node2 = set_next(id_union, node2)) {

                Neighbor *n1 = (Neighbor *)set_node_value(id_union, node1);
                Neighbor *n2 = (Neighbor *)set_node_value(id_union, node2);

                int id1 = *(n1->getid());
                int id2 = *(n2->getid());

                if (id1 == id2)
                    continue;

                if ((n1->getFlag() == 1) || (n2->getFlag() == 1)) {    // if one of the two neighbors is new, go for it
                    Vertex *v1 = vertexArray[id1];
                    Vertex *v2 = vertexArray[id2];

                    double dist;

                    DataType *data1 = static_cast<DataType *>(v1->getData());
                    DataType *data2 = static_cast<DataType *>(v2->getData());

                    dist = distance(data1, data2, dimensions);
                    n1->setFalse();   // later, set one of them false 

                    Neighbor *furthest = furthest_neighbor(v1->getNeighbors());
                    if (dist < *(furthest->getDistance()))
                    {
                        Neighbor *newNeighbor = new Neighbor(id2, dist);
                        if (set_find_node(v1->getNeighbors(), newNeighbor) != NULL) {
                            // the pontential neighbor we are about to insert is already a neighbor of id1, so we skip this part
                            delete newNeighbor;
                        }
                        else {
                            v1->addPotentialNeighbor(newNeighbor);
                        }
                    }
                }
            }
        }
        set_destroy(id_union);
    }
}

template <typename DataType, typename DistanceFunction>
int KNNDescent<DataType, DistanceFunction>::updateGraph()
{
    int updated = 0;

    // for every vertex in the graph
    for (int i = 0; i < size; i++)
    {
        Set nn = vertexArray[i]->getNeighbors();
        Set pn = vertexArray[i]->getPotentialNeighbors();

        if (set_size(pn) == 0) // if there are no potential neighbors for update, move to the next vertex
            continue;

        Neighbor *closestPotential = closest_neighbor(pn);
        int closestPotentialId = *closestPotential->getid();
        double closestPotentialDistance = *closestPotential->getDistance();

        Neighbor *furthestNeighbor = furthest_neighbor(nn);
        int furthestNeighborId = *furthestNeighbor->getid();
        double furthestNeighborDistance = *furthestNeighbor->getDistance();

        // keep updating the neighbors while there is room for update: while there are potential neighbors that are closer to the node than the furthest current neighbor, do the update
        while (closestPotentialDistance < furthestNeighborDistance)
        {
            Neighbor *newNeighbor = new Neighbor(closestPotentialId, closestPotentialDistance);
            set_insert(nn, newNeighbor); // placing the new neighbor in the set
            updated++;

            set_remove(nn, furthestNeighbor); // removing the furthest one
            set_remove(pn, closestPotential); // updating the potential neighbor set

            // restore the reverse neighbors of the vertex
            Neighbor reverse_to_remove(i, furthestNeighborDistance);
            bool removed = set_remove(vertexArray[furthestNeighborId]->getReverseNeighbors(), &reverse_to_remove);

            Neighbor *new_reverse = new Neighbor(i, closestPotentialDistance);
            vertexArray[closestPotentialId]->addReverseNeighbor(new_reverse);

            if (set_size(pn) == 0)
                break;

            closestPotential = closest_neighbor(pn);
            closestPotentialDistance = *closestPotential->getDistance();
            closestPotentialId = *closestPotential->getid();

            furthestNeighbor = furthest_neighbor(nn);
            furthestNeighborDistance = *furthestNeighbor->getDistance();
            furthestNeighborId = *furthestNeighbor->getid();
        }
        vertexArray[i]->resetPNNSet();
    }                
    double d = 0.001;
    double term = d*K*size;
    cout << "Updated " << updated << " neighbors - " << term << endl;
    if(updated < term)
        return 0;
    return updated;
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::createKNNGraph()
{

    for (int i = 0; i < 10; i++)
    {
        cout << "iteration " << i << endl;
        calculatePotentialNewNeighbors();
        if (updateGraph() == 0)
            break;
    }
}

template <typename DataType, typename DistanceFunction>
int **KNNDescent<DataType, DistanceFunction>::extract_neighbors_to_list()
{
    int **neighbors = new int *[size];
    for (int i = 0; i < size; i++)
    {
        neighbors[i] = new int[K];
    }

    for (int i = 0; i < size; i++)
    {
        Vertex *v = vertexArray[i];
        Set nn = v->getNeighbors();

        int j = 0;
        for (SetNode node = set_first(nn); node != SET_EOF; node = set_next(nn, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(nn, node);
            neighbors[i][j] = *n->getid();
            j++;
        }
    }

    return neighbors;
}

template <typename DataType, typename DistanceFunction>
void **KNNDescent<DataType, DistanceFunction>::NNSinglePoint(void *data)
{
    createKNNGraph();

    DataType *queryData = static_cast<DataType *>(data);
    void **nearest_neighbor_data_array;

    for (int i = 0; i < size; i++)
    {
        Vertex *v = vertexArray[i];
        DataType *vertexData = static_cast<DataType *>(v->getData());

        double dist = distance(vertexData, queryData, dimensions);
        if (dist == 0.0) // found the query data point
        {
            Set nn = v->getNeighbors();
            int neighbors_size = set_size(nn);
            nearest_neighbor_data_array = new void *[neighbors_size];
            int j = 0;
            for (SetNode node = set_first(nn); node != SET_EOF; node = set_next(nn, node))
            {
                Neighbor *n = (Neighbor *)set_node_value(nn, node);
                int neighbor_id = *n->getid();
                nearest_neighbor_data_array[i] = vertexArray[neighbor_id]->getData();
                j++;
            }
            break;
        }
    }

    return nearest_neighbor_data_array;
}
