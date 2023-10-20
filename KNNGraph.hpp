#include <iostream>
// #include <unordered_set>
#include "ADTMap.h"
using namespace std;

int compare_ints(Pointer a, Pointer b);

class DataPoint
{
private:
    int id;
    void *datapoint;

public:
    DataPoint(int _id, void *_datapoint);
    int getindex();
    void *getdata();
};

class Vertex
{
private:
    DataPoint *data;
    Map NN;  // structure that holds the nearest neighbors of this point (key-data_id, value-pointer to the actual data)
    Map RNN; // structure that holds the reverse nearest neighbors of this point (same logic applies)

public:
    Vertex(DataPoint *_data);
    DataPoint *getpointer();
    Map getNeighbors();
    Map getReverseNeighbors();
};

class KNNGraph
{
private:
    int k;
    int dataset_size;
    Vertex **vertexArray; // implementing an adjacency set

public:
    KNNGraph(int _k, int size, void **dataset);
    ~KNNGraph();
    void *get_vertex_info(int id);
};
