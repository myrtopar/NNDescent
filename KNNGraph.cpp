#include <iostream>
#include "KNNGraph.hpp"

using namespace std;

int compare_ints(Pointer a, Pointer b) {
    return *(int *)a - *(int *)b;
}

DataPoint::DataPoint(int _id, void *_datapoint) : id(_id), datapoint(_datapoint) {}

int DataPoint::getId() const {
    return id;
}

void* DataPoint::getAddr() const { 
    return datapoint; 
}


Vertex::Vertex(DataPoint* _data) : data(_data) {
    // for each vertex, create a map that holds its nearest neighbors and reverse neighbors
    // (key->id, value->pointer to the actual data)
    NN = map_create(compare_ints, NULL, NULL);
    map_set_hash_function(NN, hash_pointer);
    map_set_destroy_key(NN, NULL);
    map_set_destroy_value(NN, NULL);

    RNN = map_create(compare_ints, NULL, NULL);
    map_set_hash_function(RNN, hash_pointer);
    map_set_destroy_key(RNN, NULL);
    map_set_destroy_value(RNN, NULL);

    cout << "Constructed a vertex ---> ";
    cout << data->getId() << " . " << data->getAddr() << "\n";
}


void Vertex::addNeighbor(Vertex* neighborVertex) {
    map_insert(NN, (Pointer)(intptr_t)neighborVertex->data->getId(), neighborVertex->data); // neighbors
    map_insert(neighborVertex->RNN, (Pointer)(intptr_t)data->getId(), data); // reverse neighbors
} 


Map Vertex::getNeighbors() const {
    return NN;
}

Map Vertex::getReverseNeighbors() const {
    return RNN;
}



KNNGraph::KNNGraph(int _K, int _size, MyTuple* myTuples) : size(_size), K(_K) {
    cout << "\nConstructing a graph of " << size << " elements" << endl;
    vertexArray = new Vertex*[size];
    for (int i = 0; i < size; i++) {
        vertexArray[i] = new Vertex(new DataPoint(i, &myTuples[i]));
    }
    createRandomGraph(K, vertexArray);
}


// this function gets an array of datapoints and connects them (in respect to K) randomly
void KNNGraph::createRandomGraph(int K, Vertex **vertexArray) {
    cout << "\nInitializing the graph..." << endl;
    // Connect each vertex with K random neighbors
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < K; j++) {
            int randomNeighborIndex;
            do {
                randomNeighborIndex = rand() % size;
            } while (randomNeighborIndex == i);
            vertexArray[i]->addNeighbor(vertexArray[randomNeighborIndex]);
        }
    }
    printf("Inserted all neighbors and reverse neighbors\n");
}

void KNNGraph::printNeighbors() const {
    cout << "\nPrinting Neighbors:" << endl;
    for (int i = 0; i < size; i++) {
        Vertex* vertex = vertexArray[i];
        cout << "Vertex " << i << " neighbors: ";
        Map neighbors = vertex->getNeighbors();
        
        // Iterate through the neighbors and print their IDs
        MapNode node = map_first(neighbors);
        while (node != MAP_EOF) {
            int neighborId = (int)(intptr_t)map_node_key(neighbors, node);
            cout << neighborId << " ";
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
            int neighborId = (int)(intptr_t)map_node_key(neighbors, node);
            cout << neighborId << " ";
            node = map_next(neighbors, node);
        }
        
        cout << endl;
    }
}


KNNGraph::~KNNGraph() {
    for (int i = 0; i < size; i++) {
        delete vertexArray[i]; // Delete each Vertex
    }
    delete[] vertexArray; // Delete the array of Vertex pointers
}