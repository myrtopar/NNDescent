#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include "ADTSet.h"
#include "classes.hpp"

using namespace std;

// due to the template usage, the implementation of the functions below should be available in this file

template <typename DataType, typename DistanceFunction>
class KNNDescent
{
private:
    int K;
    int size;
    int dimensions;
    DistanceFunction distance;

public:
    KNNDescent(int _K, int _size, int dimensions, DataType **myTuples, DistanceFunction _distance);

    void createRandomGraph(int K, Vertex **vertexArray);
    void printNeighbors() const;
    void printReverseNeighbors() const;
    void printPotentialNeighbors() const;
    void calculatePotentialNewNeighbors();
    int updateGraph();
    void createKNNGraph();
    int **extract_neighbors_to_list();

    void test_update();
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

    void printNeighborsBF() const;
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
            // cout << "j = " << j << " neighbor id: " << *p->getid() << " distance: " << *p->getDistance() << endl;
            Neighbor *newNeighbor = new Neighbor(*p->getid(), *p->getDistance());
            set_insert(Kneighbors, newNeighbor);
            j++;
        }
        vertex->replaceNNSet(Kneighbors);
    }
}

template <typename DataType, typename DistanceFunction>
void KNNBruteForce<DataType, DistanceFunction>::printNeighborsBF() const
{
    cout << "\nPrinting Nearest Neighbors:" << endl;

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        cout << "Vertex " << i << " nearest neighbors: ";
        Set Neighbors = vertex->getNeighbors();

        int j = 0;
        for (SetNode node = set_first(Neighbors); node != SET_EOF; node = set_next(Neighbors, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(Neighbors, node);
            int *id = n->getid();
            double *dist = n->getDistance();
            cout << "\e[1;32m" << *id << "\e[0m"
                 << "(" << *dist << "), ";
        }
        cout << endl;
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

/////////////////KNNDESCENT
template <typename DataType, typename DistanceFunction>
KNNDescent<DataType, DistanceFunction>::KNNDescent(int _K, int _size, int _dimensions, DataType **data, DistanceFunction _metricFunction) : K(_K), size(_size), dimensions(_dimensions), distance(_metricFunction)
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
            // vertexArray[i]->addDistance(randomNeighborIndex, dist);

            Neighbor *newReverseNeighbor = new Neighbor(i, dist);
            vertexArray[randomNeighborIndex]->addReverseNeighbor(newReverseNeighbor);
        }
        set_destroy(usedIds);
    }
}

// 	for (MapNode node = map_first(map); node != MAP_EOF; node = map_next(map, node)) {

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
        int arraysize = set_size(neighbors) + set_size(reverseNeighbors);
        int idArray[arraysize];

        int x = 0;
        for (SetNode node = set_first(neighbors); node != SET_EOF; node = set_next(neighbors, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(neighbors, node);
            int n_id = *(n->getid());
            idArray[x] = n_id;
            x++;
        }

        for (SetNode node = set_first(reverseNeighbors); node != SET_EOF; node = set_next(reverseNeighbors, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(reverseNeighbors, node);
            int rn_id = *(n->getid());
            idArray[x] = rn_id;
            x++;
        }

        // local join in sets neighborArray and ReverseNeighborArray

        for (int j = 0; j < arraysize; j++)
        {
            for (int k = 0; k < arraysize; k++)
            {
                int id1 = idArray[j];
                int id2 = idArray[k];

                if (id1 == id2)
                    continue;

                // cout << "comparing " << id1 << " and " << id2;
                Vertex *v1 = vertexArray[id1];
                Vertex *v2 = vertexArray[id2];

                double dist;
                // void *value = map_find(v1->getDistances(), &id2);
                // if (value != NULL)
                // {
                //     // cout << "distance from " << id1 << " to " << id2 << " is already calculated: " << *(double *)value << endl;
                //     dist = *(double *)value;
                // }
                // else
                // {

                DataType *data1 = static_cast<DataType *>(v1->getData());
                DataType *data2 = static_cast<DataType *>(v2->getData());

                dist = distance(data1, data2, dimensions);
                //}

                Neighbor *furthest = furthest_neighbor(vertexArray[id1]->getNeighbors());
                if (dist > *(furthest->getDistance()))
                {
                    continue;
                }

                // cout << " - distance: " << dist << endl;
                // cout << "adding to " << id1 << " potential neighbor " << id2 << " with distance " << dist << endl;
                Neighbor *newNeighbor = new Neighbor(id2, dist);
                if (set_find_node(vertexArray[id1]->getNeighbors(), newNeighbor) != NULL)
                {
                    // the pontential neighbor we are about to insert is already a neighbor of id1, so we skip this part
                    // cout << "vertex " << id1 << " already has " << id2 << " as a neighbor" << endl;
                    delete newNeighbor;
                }
                else
                {
                    v1->addPotentialNeighbor(newNeighbor);
                }
            }
        }
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
        double closestPotentialDistance = *closestPotential->getDistance();

        Neighbor *furthestNeighbor = furthest_neighbor(nn);
        double furthestNeighborDistance = *furthestNeighbor->getDistance();

        // keep updating the neighbors while there is room for update: while there are potential neighbors that are closer to the node than the furthest current neighbor, do the update
        while (closestPotentialDistance < furthestNeighborDistance)
        {

            Neighbor *newNeighbor = new Neighbor(*closestPotential->getid(), *closestPotential->getDistance());
            set_insert(nn, newNeighbor); // placing the new neighbor in the set
            updated++;

            set_remove(nn, furthestNeighbor); // removing the furthest one
            set_remove(pn, closestPotential); // updating the potential neighbor set

            if (set_size(pn) == 0)
                break;

            closestPotential = closest_neighbor(pn);
            closestPotentialDistance = *closestPotential->getDistance();
            int closestPotentialId = *closestPotential->getid();

            furthestNeighbor = furthest_neighbor(nn);
            furthestNeighborDistance = *furthestNeighbor->getDistance();
            int furthestNeighborId = *furthestNeighbor->getid();
        }

        vertexArray[i]->resetPNNSet();
        vertexArray[i]->resetRNNSet();

        // RESTORE THE REVERSE NEIGBORS OF THE VERTEX
        for (SetNode node = set_first(nn); node != SET_EOF; node = set_next(nn, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(nn, node);
            int nid = *n->getid();
            double ndist = *n->getDistance();

            Neighbor *newReverse = new Neighbor(i, ndist);
            vertexArray[nid]->addReverseNeighbor(newReverse);
        }
    }

    return updated;
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::createKNNGraph()
{

    for (int i = 0; i < 10; i++)
    {
        calculatePotentialNewNeighbors();
        if (updateGraph() == 0)
            break;
    }
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::test_update()
{
    int flag = 0;
    cout << "\nIN TEST UPDATE\n"
         << endl;

    int i;
    for (i = 0; i < size; i++)
    {
        Vertex *v = vertexArray[i];

        Set nn = v->getNeighbors();

        int j = 0;
        for (SetNode node = set_first(nn); node != SET_EOF; node = set_next(nn, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(nn, node);
            int id = *(n->getid());
            double dist1 = *(n->getDistance());
            Vertex *nv = vertexArray[id];

            DataType *data1 = static_cast<DataType *>(v->getData());
            DataType *data2 = static_cast<DataType *>(nv->getData());

            double dist2 = distance(data1, data2, dimensions);

            // cout << "vertex " << i << " neighbor " << id << endl;
            // cout << "distance from set: " << dist1 << ", distance from calculation: " << dist2 << endl;

            if (dist1 != dist2)
            {
                flag = 1;
                cout << "\e[31mERROR! VERTEX " << i << " AND NEIGHBOR VERTEX " << id << " HAVE CONFLICTING DISTANCES!\e[0m" << endl;
                cout << "set distance between " << i << " and " << id << ": " << dist1 << endl;
                cout << "real distance between " << i << " and " << id << ": " << dist2 << endl;
            }
        }
        // cout << endl;
    }

    for (i = 0; i < size; i++)
    {
        Vertex *v = vertexArray[i];

        Set rnn = v->getReverseNeighbors();

        int j = 0;
        for (SetNode node = set_first(rnn); node != SET_EOF; node = set_next(rnn, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(rnn, node);
            int id = *(n->getid());
            double dist1 = *(n->getDistance());
            Vertex *nv = vertexArray[id];

            DataType *data1 = static_cast<DataType *>(v->getData());
            DataType *data2 = static_cast<DataType *>(nv->getData());

            double dist2 = distance(data1, data2, dimensions);

            // cout << "vertex " << i << " neighbor " << id << endl;
            // cout << "distance from set: " << dist1 << ", distance from calculation: " << dist2 << endl;

            if (dist1 != dist2)
            {
                flag = 1;
                cout << "\e[31mERROR! VERTEX " << i << " AND REVERSE NEIGHBOR VERTEX " << id << " HAVE CONFLICTING DISTANCES!\e[0m" << endl;
                cout << "set distance between " << i << " and " << id << ": " << dist1 << endl;
                cout << "real distance between " << i << " and " << id << ": " << dist2 << endl;
            }
        }
        // cout << endl;
    }

    for (i = 0; i < size; i++)
    {
        Vertex *v = vertexArray[i];

        Set pnn = v->getPotentialNeighbors();

        int j = 0;
        for (SetNode node = set_first(pnn); node != SET_EOF; node = set_next(pnn, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(pnn, node);
            int id = *(n->getid());
            double dist1 = *(n->getDistance());
            Vertex *nv = vertexArray[id];

            DataType *data1 = static_cast<DataType *>(v->getData());
            DataType *data2 = static_cast<DataType *>(nv->getData());

            double dist2 = distance(data1, data2, dimensions);

            // cout << "vertex " << i << " neighbor " << id << endl;
            // cout << "distance from set: " << dist1 << ", distance from calculation: " << dist2 << endl;

            if (dist1 != dist2)
            {
                flag = 1;
                cout << "\e[31mERROR! VERTEX " << i << " AND POTENTIAL NEIGHBOR VERTEX " << id << " HAVE CONFLICTING DISTANCES!\e[0m" << endl;
                cout << "set distance between " << i << " and " << id << ": " << dist1 << endl;
                cout << "real distance between " << i << " and " << id << ": " << dist2 << endl;
            }
        }
        // cout << endl;
    }
    if (flag == 0)
    {
        cout << "\e[32mPassed the tests\e[0m" << endl;
    }
    else
    {
        cout << "\e[31mDid not pass the tests\e[0m" << endl;
    }
}

template <typename DataType, typename DistanceFunction>
int **KNNDescent<DataType, DistanceFunction>::extract_neighbors_to_list()
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
void KNNDescent<DataType, DistanceFunction>::printNeighbors() const
{
    cout << "\nPrinting Nearest Neighbors:" << endl;

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        cout << "Vertex " << i << " nearest neighbors: ";
        Set Neighbors = vertex->getNeighbors();

        int j = 0;
        for (SetNode node = set_first(Neighbors); node != SET_EOF; node = set_next(Neighbors, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(Neighbors, node);
            int *id = n->getid();
            double *dist = n->getDistance();
            cout << "\e[1;32m" << *id << "\e[0m"
                 << "(" << *dist << "), ";
        }
        cout << endl;
    }
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::printReverseNeighbors() const
{
    cout << "\nPrinting Reverse Neighbors:" << endl;

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        cout << "Vertex " << i << " reverse neighbors: ";
        Set rNeighbors = vertex->getReverseNeighbors();

        int j = 0;
        for (SetNode node = set_first(rNeighbors); node != SET_EOF; node = set_next(rNeighbors, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(rNeighbors, node);
            int *id = n->getid();
            double *dist = n->getDistance();
            cout << "\e[1;32m" << *id << "\e[0m"
                 << "(" << *dist << "), ";
        }
        cout << endl;
    }
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::printPotentialNeighbors() const
{
    cout << "\nPrinting Potential Neighbors:" << endl;

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        cout << "Vertex " << i << " potential neighbors: ";
        Set pNeighbors = vertex->getPotentialNeighbors();

        int j = 0;
        for (SetNode node = set_first(pNeighbors); node != SET_EOF; node = set_next(pNeighbors, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(pNeighbors, node);
            int *id = n->getid();
            double *dist = n->getDistance();
            cout << "\e[1;32m" << *id << "\e[0m"
                 << "(" << *dist << "), ";
        }
        cout << endl;
    }
}