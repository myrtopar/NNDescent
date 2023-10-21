#include <iostream>
#include <cstdlib> 
#include "ADTMap.h"

using namespace std;

int compare_ints(Pointer a, Pointer b);

struct MyTuple {
    int num1;
    int num2;
    int num3;
};

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

    void addNeighbor(Vertex* neighborVertex);
    Map getNeighbors() const;
    Map getReverseNeighbors() const;

};



class KNNGraph {
private:
    Vertex** vertexArray;
    int size;
    int K;

public:
    KNNGraph(int _K, int _size, MyTuple* myTuples);

    void createRandomGraph(int K, Vertex **vertexArray);
    void printNeighbors() const;

    ~KNNGraph();
};
