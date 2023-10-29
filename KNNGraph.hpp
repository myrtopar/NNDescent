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
    Vertex **vertexArray;
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
    void updateGraph();

    ~KNNDescent();
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

    ~KNNBruteForce();
};

///////////////////////////////////////////////////////////////////

template <typename DataType, typename DistanceFunction>
KNNBruteForce<DataType, DistanceFunction>::KNNBruteForce(int _K, int _size, int _dimensions, DataType **data, DistanceFunction _metricFunction) : K(_K), size(_size), dimensions(_dimensions), distance(_metricFunction)
{
    cout << "\nConstructing a BRUTEFORCE graph of " << size << " elements" << endl;
    vertexArray = new Vertex *[size];
    for (int i = 0; i < size; i++)
    {
        // cout << i << " " << &data[i] << endl;
        vertexArray[i] = new Vertex(new DataPoint(i, data[i]));
    }
    calculateKNNBF();
}

template <typename DataType, typename DistanceFunction>
void KNNBruteForce<DataType, DistanceFunction>::calculateKNNBF() const
{
    cout << "\nBrute Force on graph to create KNN..." << endl;

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        // cout << "Vertex " << i << endl;

        for (int j = 0; j < size; j++)
        {
            if (i == j)
                continue;

            // cout << "Distance from vertex " << j;
            Vertex *nvertex = vertexArray[j];

            DataType *vertexData = static_cast<DataType *>(vertex->getData()->getAddr());
            DataType *neighborData = static_cast<DataType *>(nvertex->getData()->getAddr());

            double dist = distance(vertexData, neighborData, dimensions);
            // cout << ": " << dist << endl;

            Neighbor *newNeighbor = new Neighbor(j, dist);
            vertex->addNeighbor(newNeighbor);
        }
    }

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        // cout << "vertex " << i << endl;
        for (int j = 0; j < size - K - 1; j++)
        {
            SetNode s = set_last(vertex->getNeighbors());
            Neighbor *p = (Neighbor *)set_node_value(vertex->getNeighbors(), s);
            set_remove(vertex->getNeighbors(), p);
        }
    }

    printNeighborsBF();
}

template <typename DataType, typename DistanceFunction>
void KNNBruteForce<DataType, DistanceFunction>::printNeighborsBF() const
{
    cout << "\nPrinting Neighbors:" << endl;

    for (int i = 0; i < size; i++)
    {

        Vertex *vertex = vertexArray[i];
        cout << "Vertex " << i << " neighbors: ";
        Set neighbors = vertex->getNeighbors();
        void **nArray = set_to_array(neighbors);

        for (int j = 0; j < set_size(neighbors); j++)
        {
            Neighbor *n = (Neighbor *)nArray[j];
            int *id = n->getid();
            double *dist = n->getDistance();
            cout << *id << " - " << *dist << ", ";
        }

        free(nArray);
        cout << endl;
    }
}

template <typename DataType, typename DistanceFunction>
KNNBruteForce<DataType, DistanceFunction>::~KNNBruteForce()
{
    for (int i = 0; i < size; i++)
    {
        delete vertexArray[i]; // Delete each Vertex
    }
    delete[] vertexArray; // Delete the array of Vertex pointers
}

template <typename DataType, typename DistanceFunction>
KNNDescent<DataType, DistanceFunction>::KNNDescent(int _K, int _size, int _dimensions, DataType **data, DistanceFunction _metricFunction) : K(_K), size(_size), dimensions(_dimensions), distance(_metricFunction)
{
    cout << "\nConstructing a graph of " << size << " elements" << endl;
    vertexArray = new Vertex *[size];
    for (int i = 0; i < size; i++)
    {
        // cout << "data " << i << " address: " << data[i] << endl;
        vertexArray[i] = new Vertex(new DataPoint(i, data[i]));
    }
    createRandomGraph(K, vertexArray);
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::createRandomGraph(int K, Vertex **vertexArray)
{
    cout << "\nInitializing the graph..." << endl;

    // Connect each vertex with K random neighbors
    for (int i = 0; i < size; i++)
    {
        cout << "Vertex " << i << endl;
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

            cout << j << " random index: " << randomNeighborIndex << endl;
            int *randomId = create_int(randomNeighborIndex);
            set_insert(usedIds, randomId);

            // calculate distance
            DataType *vertexData = static_cast<DataType *>(vertexArray[i]->getData()->getAddr());
            DataType *neighborData = static_cast<DataType *>(vertexArray[randomNeighborIndex]->getData()->getAddr());

            double dist = distance(vertexData, neighborData, dimensions);

            Neighbor *newNeighbor = new Neighbor(randomNeighborIndex, dist);
            vertexArray[i]->addNeighbor(newNeighbor);

            Neighbor *newReverseNeighbor = new Neighbor(i, dist);
            vertexArray[randomNeighborIndex]->addReverseNeighbor(newReverseNeighbor);
        }
        set_destroy(usedIds);

        cout << endl;
    }
    printf("Inserted all neighbors and reverse neighbors\n");
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
            cout << *id << "(" << *dist << "), ";
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
        Set rneighbors = vertex->getReverseNeighbors();

        void **nArray = set_to_array(rneighbors);
        for (int j = 0; j < set_size(rneighbors); j++)
        {
            Neighbor *n = (Neighbor *)nArray[j];
            int *id = n->getid();
            cout << *id << " ";
        }
        free(nArray);

        cout << endl;
    }

    cout << endl;
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
            cout << *id << "(" << *dist << "), ";
        }
        cout << endl;
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
    cout << "\nCalculate KNN..." << endl;
    // Every calculatePotentialNewNeighbors call calculates the potential neighbors of each vector and prepares the graph for updates.
    // Following the "friend of a friend" logic (local join between the NN and RNN sets), this call counts as one iteration through the vertices of the graph

    // for every vertex, we find its potential neighbors for update
    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        cout << "Vertex " << i << endl;

        // remove_all(vertexArray[i]->getPotentialNeighbors()); // empty this set if there are neighbors from previous iterations

        Set neighbors = vertex->getNeighbors();
        void **neighborArray = set_to_array(neighbors); // array me nn tou vertex

        Set ReverseNeighbors = vertex->getReverseNeighbors();
        void **ReverseNeighborArray = set_to_array(ReverseNeighbors); // array me rnn tou vertex

        // local join in sets neighborArray and ReverseNeighborArray
        for (int j = 0; j < set_size(neighbors); j++)
        {
            Neighbor *n = (Neighbor *)neighborArray[j];
            int n_id = *(n->getid());
            Vertex *v1 = vertexArray[n_id];
            // extracting the data from the neighbor vertex
            DataType *neighborData = static_cast<DataType *>(v1->getData()->getAddr());

            for (int k = 0; k < set_size(ReverseNeighbors); k++)
            {
                cout << "neighbor " << n_id;
                Neighbor *rn = (Neighbor *)ReverseNeighborArray[k];
                int rn_id = *(rn->getid());
                cout << " and reverse neighbor " << rn_id;
                if (n_id == rn_id)
                {
                    // cannot compare a vertex with itself, so we continue to the next reverse neighbor
                    cout << " have zero distance." << endl;
                    continue;
                }

                Vertex *v2 = vertexArray[rn_id];
                // extracting the data from the reverse neighbor vertex
                DataType *reverseNeighborData = static_cast<DataType *>(v2->getData()->getAddr());

                double dist = distance(neighborData, reverseNeighborData, dimensions);

                cout << " have distance " << dist << endl;

                Neighbor *newPotentialNeighbor = new Neighbor(rn_id, dist);
                if (set_find_node(vertexArray[n_id]->getNeighbors(), newPotentialNeighbor) != NULL)
                {
                    // the pontential neighbor we are about to insert, is already a neighbor of n_id so we skip
                    delete newPotentialNeighbor;
                    continue;
                }
                cout << "For vertex " << n_id << ", creating new potential neighbor with id " << rn_id << " and distance " << dist << endl;
                vertexArray[n_id]->addPotentialNeighbor(newPotentialNeighbor);
            }
        }

        free(neighborArray);
        free(ReverseNeighborArray);

        cout << endl;
    }
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::updateGraph()
{

    cout << endl;
    cout << "/////////////// UPDATING GRAPH //////////////" << endl;
    // for every vertex in the graph
    for (int i = 0; i < size; i++)
    {
        cout << "vertex " << i << ": " << endl;
        // will place the updated neighbors in this set and replace the old NN set with this one
        Set newNeighborSet = set_create(compare_distances, NULL);

        Set nn = vertexArray[i]->getNeighbors();
        Set pn = vertexArray[i]->getPotentialNeighbors();

        // for every neighbor of the vertex
        int j = 0;
        for (SetNode node = set_first(nn); node != SET_EOF; node = set_next(nn, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(nn, node);
            Neighbor *p = vertexArray[i]->closest_neighbor(pn);

            if (p == NULL) // if there is no closest potential neighbor
            {
                // copy the neighbors from the old nn set to the new nn set
                cout << "no potential neighbors for update, copying neighbor with id " << *n->getid() << " to the new set" << endl;
                Neighbor *newNeighbor = new Neighbor(*n->getid(), *n->getDistance());
                set_insert(newNeighborSet, newNeighbor);
                j++;
                continue;
            }

            cout << "neighbor with id: " << *n->getid() << " with distance " << *n->getDistance() << " and potential neighbor with id: " << *p->getid() << " with distance " << *p->getDistance() << endl;

            if (*p->getDistance() < *n->getDistance())
            {
                // if the closest potential neighbor IS CLOSER than the existing neighbor, we make the potential neighbor a new one and add it to the new set
                Neighbor *newNeighbor = new Neighbor(*p->getid(), *p->getDistance());
                cout << "replacing " << *n->getid() << " with " << *p->getid() << endl;
                set_insert(newNeighborSet, newNeighbor);
                set_remove(pn, p);
            }
            else
            {
                // copy the neighbor from the old nn set to the new nn set
                Neighbor *newNeighbor = new Neighbor(*n->getid(), *n->getDistance());
                set_insert(newNeighborSet, newNeighbor);
            }
            j++;
        }
        cout << endl;
        vertexArray[i]->replaceNNSet(newNeighborSet);
        remove_all(pn); // remove all the potential neighbors after the update for next use
    }
}
