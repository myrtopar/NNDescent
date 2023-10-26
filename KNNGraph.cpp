#include "KNNGraph.hpp"

using namespace std;



int compare_ints(Pointer a, Pointer b) {
    return *(int *)a - *(int *)b;
}


int compare_distances(Pointer a, Pointer b) {
    Neighbor *n1 = (Neighbor*)a;
    Neighbor *n2 = (Neighbor*)b;

    double *distance1 = n1->getDistance();
    double *distance2 = n2->getDistance();
    
    // cout << "D1: " << *distance1 << " D2: " << *distance2 << endl;

    if(*distance1 != *distance2) 
        return *distance1 - *distance2;
    
    return *(int *)n1->getid() - *(int *)n2->getid();
}

DataPoint::DataPoint(int _id, void *_datapoint) : id(_id), datapoint(_datapoint) {}

int DataPoint::getId() const {
    return id;
}

void* DataPoint::getAddr() const { 
    return datapoint; 
}


Vertex::Vertex(DataPoint* _data) : data(_data) {
    NN = pqueue_create(compare_distances, NULL, NULL);
    pqueue_set_destroy_value(NN, NULL);

    RNN = pqueue_create(compare_distances, NULL, NULL);
    pqueue_set_destroy_value(RNN, NULL);
}


void Vertex::addNeighbor(Neighbor* neighbor) {
    pqueue_insert(NN, neighbor);

} 

void Vertex::addReverseNeighbor(Neighbor *neighbor){
    pqueue_insert(RNN, neighbor);
}



PriorityQueue Vertex::getNeighbors() const {
    return NN;
}

PriorityQueue Vertex::getReverseNeighbors() const {
    return RNN;
}

DataPoint *Vertex::getData() const {
    return data;
}


Neighbor::Neighbor(int _id, double _distance) {
    id = new int;
    *id = _id;   
    distance = new double;
    *distance = _distance;    
    // cout << "Constructed neighbor no:" << _id << " with distance:" << _distance << " \n";

}

int *Neighbor::getid(){
    return id;
}

double *Neighbor::getDistance(){
    return distance;
}

