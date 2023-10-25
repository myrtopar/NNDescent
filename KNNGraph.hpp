#include <iostream>
#include <cstdlib>
#include "ADTMap.h"

using namespace std;

int compare_ints(Pointer a, Pointer b);

struct MyTuple
{
    int num1;
    int num2;
    int num3;
};

// typedef double (*DistanceFunction)(const void* a, const void* b);

typedef double (*DistanceMetric)(const void *a, const void *b);

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
    Map NN;
    Map RNN;

public:
    Vertex(DataPoint *_data);

    void addNeighbor(Vertex *neighborVertex);
    Map getNeighbors() const;
    Map getReverseNeighbors() const;
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

    ~KNNGraph();
};

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
            } while (randomNeighborIndex == i);
            vertexArray[i]->addNeighbor(vertexArray[randomNeighborIndex]);
        }
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
        Map neighbors = vertex->getNeighbors();

        // Iterate through the neighbors and print their IDs
        MapNode node = map_first(neighbors);
        while (node != MAP_EOF)
        {
            int neighborId = (int)(intptr_t)map_node_key(neighbors, node);
            cout << neighborId << " ";
            node = map_next(neighbors, node);
        }

        cout << endl;
    }

    cout << "\nPrinting Reverse Neighbors:" << endl;
    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        cout << "Vertex " << i << " reverse neighbors: ";
        Map neighbors = vertex->getReverseNeighbors();

        // Iterate through the neighbors and print their IDs
        MapNode node = map_first(neighbors);
        while (node != MAP_EOF)
        {
            int neighborId = (int)(intptr_t)map_node_key(neighbors, node);
            cout << neighborId << " ";
            node = map_next(neighbors, node);
        }

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