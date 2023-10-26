#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include "ADTPriorityQueue.h"
#include "ADTSet.h"

using namespace std;

class Neighbor;

int compare_ints(Pointer a, Pointer b);

struct MyTuple
{
    int num1;
    int num2;
    int num3;
};

// typedef double (*DistanceFunction)(const void* a, const void* b);

class DataPoint
{
private:
    int id;
    void *datapoint;

public:
    DataPoint(int _id, void *_datapoint);

    int getId() const;
    void *getAddr() const;
};

class Vertex
{
private:
    DataPoint *data;
    Set NN;
    Set RNN;
    Set potentialNN;

public:
    Vertex(DataPoint *_data);

    DataPoint *getData() const;
    void addNeighbor(Neighbor *neighbor);
    void addReverseNeighbor(Neighbor *neighbor);
    void addPotentialNeighbor(Neighbor *neighbor);
    Set getNeighbors() const;
    Set getReverseNeighbors() const;
    Set getPotentialNeighbors() const;
};

class Neighbor
{
private:
    int *id;
    double *distance;
    int flag;

public:
    Neighbor(int _id, double _distance);
    int *getid();
    double *getDistance();
};

template <typename DataType, typename DistanceFunction>
class KNNGraph
{
private:
    Vertex **vertexArray;
    int K;
    int size;
    DistanceFunction distance;

public:
    KNNGraph(int _K, int _size, DataType *myTuples, DistanceFunction _distance);

    void createRandomGraph(int K, Vertex **vertexArray);
    void printNeighbors() const;
    void printPotentialNeighbors() const;
    void calculateKNN() const;

    ~KNNGraph();
};

template <typename DataType, typename DistanceFunction>
class KNNGraphBruteForce
{
private:
    Vertex **vertexArray;
    int K;
    int size;
    int dimensions;
    DistanceFunction distance;

public:
    KNNGraphBruteForce(int _K, int _size, int dimensions, DataType **data, DistanceFunction _distance);

    void printNeighborsBF() const;
    void calculateKNNBF() const;

    ~KNNGraphBruteForce();
};

template <typename DataType, typename DistanceFunction>
KNNGraphBruteForce<DataType, DistanceFunction>::KNNGraphBruteForce(int _K, int _size, int _dimensions, DataType **data, DistanceFunction _metricFunction) : K(_K), size(_size), dimensions(_dimensions), distance(_metricFunction)
{
    cout << "\nConstructing a BRUTEFORCE graph of " << size << " elements" << endl;
    vertexArray = new Vertex *[size];
    for (int i = 0; i < size; i++)
    {
        cout << i << " " << &data[i] << endl;
        vertexArray[i] = new Vertex(new DataPoint(i, data[i]));
    }
    calculateKNNBF();
}

template <typename DataType, typename DistanceFunction>
void KNNGraphBruteForce<DataType, DistanceFunction>::calculateKNNBF() const
{
    cout << "\nBrute Force on graph to create KNN..." << endl;

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        cout << "Vertex " << i << endl;
        for (int j = 0; j < size; j++)
        {
            if (i == j)
                continue;

            cout << "Distance from vertex " << j;
            Vertex *nvertex = vertexArray[j];

            DataType *vertexData = static_cast<DataType *>(vertex->getData()->getAddr());
            DataType *neighborData = static_cast<DataType *>(nvertex->getData()->getAddr());

            double dist = distance(vertexData, neighborData, dimensions);
            cout << ": " << dist << endl;

            Neighbor *newNeighbor = new Neighbor(j, dist);
            vertex->addNeighbor(newNeighbor);
        }
    }

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        cout << "vertex " << i << endl;
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
void KNNGraphBruteForce<DataType, DistanceFunction>::printNeighborsBF() const
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
KNNGraphBruteForce<DataType, DistanceFunction>::~KNNGraphBruteForce()
{
    for (int i = 0; i < size; i++)
    {
        delete vertexArray[i]; // Delete each Vertex
    }
    delete[] vertexArray; // Delete the array of Vertex pointers
}

// due to the template usage, the implementation of the functions below should be available in this file

template <typename DataType, typename DistanceFunction>
KNNGraph<DataType, DistanceFunction>::KNNGraph(int _K, int _size, DataType *myTuples, DistanceFunction _metricFunction) : K(_K), size(_size), distance(_metricFunction)
{
    cout << "\nConstructing a graph of " << size << " elements" << endl;
    vertexArray = new Vertex *[size];
    for (int i = 0; i < size; i++)
    {
        vertexArray[i] = new Vertex(new DataPoint(i, &myTuples[i]));
    }
    createRandomGraph(K, vertexArray);
}

template <typename DataType, typename DistanceFunction>
void KNNGraph<DataType, DistanceFunction>::createRandomGraph(int K, Vertex **vertexArray)
{
    cout << "\nInitializing the graph..." << endl;

    // Connect each vertex with K random neighbors
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < K; j++)
        {
            int randomNeighborIndex;
            do
            {
                randomNeighborIndex = rand() % size;
            } while (randomNeighborIndex == i); // edw thelei allagh --> while (randomNeighborIndex == i || vertexArray[i]->findNeighbor(randomNeighborIndex) == 1);

            // calculate distance
            DataType *vertexData = static_cast<DataType *>(vertexArray[i]->getData()->getAddr());
            DataType *neighborData = static_cast<DataType *>(vertexArray[randomNeighborIndex]->getData()->getAddr());

            cout << "Vertex Data: (" << vertexData->num1 << ", " << vertexData->num2 << ", " << vertexData->num3 << ") ,";
            cout << "  Neighbor Data: (" << neighborData->num1 << ", " << neighborData->num2 << ", " << neighborData->num3 << ")";

            double dist = distance(*vertexData, *neighborData);
            cout << "  -   Distance: " << dist << endl;

            Neighbor *newNeighbor = new Neighbor(randomNeighborIndex, dist);
            Neighbor *newReverseNeighbor = new Neighbor(i, dist);

            // cout << "adding neighbor no " << randomNeighborIndex << " in the nn PriorityQueue of " << i << endl;
            vertexArray[i]->addNeighbor(newNeighbor);
            // cout << "adding reverse neighbor no " << i << " in the rnn PriorityQueue of " << randomNeighborIndex << endl;
            vertexArray[randomNeighborIndex]->addReverseNeighbor(newReverseNeighbor);
        }

        cout << endl;
    }
    printf("Inserted all neighbors and reverse neighbors\n");
}

template <typename DataType, typename DistanceFunction>
void KNNGraph<DataType, DistanceFunction>::printNeighbors() const
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
            cout << *id << " ";
        }
        free(nArray);

        cout << endl;
    }
}

template <typename DataType, typename DistanceFunction>
void KNNGraph<DataType, DistanceFunction>::printPotentialNeighbors() const
{
    cout << "\nPrinting Potential Neighbors:" << endl;

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        cout << "Vertex " << i << " neighbors: ";
        Set pNeighbors = vertex->getPotentialNeighbors();

        void **nArray = set_to_array(pNeighbors);
        for (int j = 0; j < set_size(pNeighbors); j++)
        {
            Neighbor *n = (Neighbor *)nArray[j];
            int *id = n->getid();
            cout << *id << " ";
        }
        free(nArray);

        cout << endl;
    }
}

template <typename DataType, typename DistanceFunction>
KNNGraph<DataType, DistanceFunction>::~KNNGraph()
{
    for (int i = 0; i < size; i++)
    {
        delete vertexArray[i]; // Delete each Vertex
    }
    delete[] vertexArray; // Delete the array of Vertex pointers
}

template <typename DataType, typename DistanceFunction>
void KNNGraph<DataType, DistanceFunction>::calculateKNN() const
{
    cout << "\nCalculate KNN..." << endl;

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        cout << "Vertex " << i << endl;

        Set neighbors = vertex->getNeighbors();
        void **neighborArray = set_to_array(neighbors); // array me nn tou vertex

        Set ReverseNeighbors = vertex->getReverseNeighbors();
        void **ReverseNeighborArray = set_to_array(ReverseNeighbors); // array me rnn tou vertex

        for (int j = 0; j < set_size(neighbors); j++)
        {
            Neighbor *n = (Neighbor *)neighborArray[j];
            int n_id = *(n->getid());
            Vertex *v1 = vertexArray[n_id];
            DataType *neighborData = static_cast<DataType *>(v1->getData()->getAddr());

            for (int k = 0; k < set_size(ReverseNeighbors); k++)
            {
                cout << "neighbor " << n_id;
                Neighbor *rn = (Neighbor *)ReverseNeighborArray[k];
                int rn_id = *(rn->getid());
                cout << " and reverse neighbor " << rn_id;
                if (n_id == rn_id)
                {
                    cout << " have zero distance." << endl;
                    continue;
                }

                Vertex *v2 = vertexArray[rn_id];
                DataType *reverseNeighborData = static_cast<DataType *>(v2->getData()->getAddr());

                double dist = distance(*neighborData, *reverseNeighborData);

                cout << " have distance " << dist << endl;

                cout << "For vertex " << n_id << ", creating new potential neighbor with id " << rn_id << " and distance " << dist << endl;
                Neighbor *newPotentialNeighbor = new Neighbor(rn_id, dist);
                vertexArray[n_id]->addPotentialNeighbor(newPotentialNeighbor);
            }
            cout << "////////////////////////////////" << endl;
        }
        free(neighborArray);
        free(ReverseNeighborArray);

        cout << endl;
    }
}

// Vertex* vertex = vertexArray[i];
// cout << "Vertex " << i << endl;
// for(int j= 0; j < size; j++){
//     if(i == j)
//         continue;

//     cout << "Distance from vertex " << j;
//     Vertex *nvertex = vertexArray[j];

//     DataType* vertexData = static_cast<DataType*>(vertex->getData()->getAddr());
//     DataType* neighborData = static_cast<DataType*>(nvertex->getData()->getAddr());

//     // cout << "Vertex Data: (" << vertexData->num1 << ", " << vertexData->num2 << ", " << vertexData->num3 << ") ,";
//     // cout << "  Neighbor Data: (" << neighborData->num1 << ", " << neighborData->num2 << ", " << neighborData->num3 << ")";

//     double dist = distance(*vertexData, *neighborData);
