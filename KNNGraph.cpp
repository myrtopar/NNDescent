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

