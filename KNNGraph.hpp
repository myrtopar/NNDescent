#include <iostream>
#include <cstdlib> 
#include "ADTMap.h"

using namespace std;

class Neighbor;

int compare_ints(Pointer a, Pointer b);

struct MyTuple {
    int num1;
    int num2;
    int num3;
};

// typedef double (*DistanceFunction)(const void* a, const void* b);

class DataPoint {
private:
    int id;
    void* datapoint;

public:
    DataPoint(int _id, void* _datapoint);
    
    int getId() const;
    void* getAddr() const;
};


class Vertex {
private:
    DataPoint* data;
    Map NN;
    Map RNN;

public:
    Vertex(DataPoint* _data);

    DataPoint *getData() const;
    void addNeighbor(Neighbor *neighbor);
    void addReverseNeighbor(Neighbor *neighbor);
    Map getNeighbors() const;
    Map getReverseNeighbors() const;
    int findNeighbor(int id) const;
};

class Neighbor {
private:
    int *id;
    double *distance;
public:
    Neighbor(int _id, double _distance);
    int *getid();
    double *getDistance();
};


template <typename DataType, typename DistanceFunction>
class KNNGraph {
private:
    Vertex** vertexArray;
    int K;
    int size;
    DistanceFunction distance;

public:
    KNNGraph(int _K, int _size, DataType* myTuples, DistanceFunction _distance);

    void createRandomGraph(int K, Vertex **vertexArray);
    void printNeighbors() const;
    void calculateKNN() const; 

    ~KNNGraph();
};





// due to the template usage, the implementation of the functions below should be available in this file

template <typename DataType, typename DistanceFunction>
KNNGraph<DataType, DistanceFunction>::KNNGraph(int _K, int _size, DataType* myTuples, DistanceFunction _metricFunction) : K(_K), size(_size), distance(_metricFunction) {
    cout << "\nConstructing a graph of " << size << " elements" << endl;
    vertexArray = new Vertex*[size];
    for (int i = 0; i < size; i++) {
        vertexArray[i] = new Vertex(new DataPoint(i, &myTuples[i]));
    }
    createRandomGraph(K, vertexArray);
}


template <typename DataType, typename DistanceFunction>
void KNNGraph<DataType, DistanceFunction>::createRandomGraph(int K, Vertex **vertexArray) {
    cout << "\nInitializing the graph..." << endl;
    // Connect each vertex with K random neighbors
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < K; j++) {
            int randomNeighborIndex;
            do {
                randomNeighborIndex = rand() % size;
            } while (randomNeighborIndex == i || vertexArray[i]->findNeighbor(randomNeighborIndex) == 1);
            
            // cout << "random neighbor no " << j << ": " << randomNeighborIndex << endl;

            // calculate distance 
            DataType* vertexData = static_cast<DataType*>(vertexArray[i]->getData()->getAddr());
            DataType* neighborData = static_cast<DataType*>(vertexArray[randomNeighborIndex]->getData()->getAddr());
            
            cout << "Vertex Data: (" << vertexData->num1 << ", " << vertexData->num2 << ", " << vertexData->num3 << ") ,";
            cout << "  Neighbor Data: (" << neighborData->num1 << ", " << neighborData->num2 << ", " << neighborData->num3 << ")";

            double dist = distance(*vertexData, *neighborData);
            cout << "  -   Distance: " << dist << endl;

            Neighbor* newNeighbor = new Neighbor(randomNeighborIndex, dist);
            Neighbor* newReverseNeighbor = new Neighbor(i, dist);

            // cout << "adding neighbor no " << randomNeighborIndex << " in the nn map of " << i << endl;
            vertexArray[i]->addNeighbor(newNeighbor);
            // cout << "adding reverse neighbor no " << i << " in the rnn map of " << randomNeighborIndex << endl;
            vertexArray[randomNeighborIndex]->addReverseNeighbor(newReverseNeighbor);

        }
        // cout << "////////////////////////////////////////////////////////////////";
        cout << endl;
    }
    printf("Inserted all neighbors and reverse neighbors\n");
}


template <typename DataType, typename DistanceFunction>
void KNNGraph<DataType, DistanceFunction>::printNeighbors() const {
    cout << "\nPrinting Neighbors:" << endl;
    for (int i = 0; i < size; i++) {
        Vertex* vertex = vertexArray[i];
        cout << "Vertex " << i << " neighbors: ";
        Map neighbors = vertex->getNeighbors();
        
        // Iterate through the neighbors and print their IDs
        MapNode node = map_first(neighbors);
        while (node != MAP_EOF) {
            int *neighborId = (int *)map_node_key(neighbors, node);
            cout << *neighborId << " ";
            node = map_next(neighbors, node);
        }
        
        cout << endl;
    }

    cout << "\nPrinting Reverse Neighbors:" << endl;
    for (int i = 0; i < size; i++) {
        Vertex* vertex = vertexArray[i];
        cout << "Vertex " << i << " reverse neighbors: ";
        Map neighbors = vertex->getReverseNeighbors();
        
        // Iterate through the neighbors and print their IDs
        MapNode node = map_first(neighbors);
        while (node != MAP_EOF) {
            int *neighborId = (int *)map_node_key(neighbors, node);
            cout << *neighborId << " ";
            node = map_next(neighbors, node);
        }
        
        cout << endl;
    }
}


template <typename DataType, typename DistanceFunction>
KNNGraph<DataType, DistanceFunction>::~KNNGraph() {
    for (int i = 0; i < size; i++) {
        delete vertexArray[i]; // Delete each Vertex
    }
    delete[] vertexArray; // Delete the array of Vertex pointers
}


template <typename DataType, typename DistanceFunction>
void KNNGraph<DataType, DistanceFunction>::calculateKNN() const {
    cout << "\nCalculate KNN :" << endl;

    for (int i = 0; i < size; i++) {
        Vertex* vertex = vertexArray[i];
        cout << "Vertex " << i << " neighbors: " << endl;
        Map neighbors = vertex->getNeighbors();
        
        // Iterate through the neighbors 
        MapNode node = map_first(neighbors);
        while (node != MAP_EOF) {
            int neighborId = (int)(intptr_t)map_node_key(neighbors, node);
            cout << neighborId << " -> ";

            DataType* vertexData = static_cast<DataType*>(vertex->getData()->getAddr());
            DataType* neighborData = static_cast<DataType*>(map_node_value(neighbors, node));
            
            cout << "Vertex Data: (" << vertexData->num1 << ", " << vertexData->num2 << ", " << vertexData->num3 << ")";
            cout << "  Neighbor Data: (" << neighborData->num1 << ", " << neighborData->num2 << ", " << neighborData->num3 << ")" << endl;

            double dist = distance(*vertexData, *neighborData);
            cout << "Distance: " << dist << endl;

            node = map_next(neighbors, node);
        }
        
        cout << endl;
    }

}