#include <iostream>
#include "KNNGraph.hpp"

using namespace std;

int compare_ints(Pointer a, Pointer b)
{
    return *(int *)a - *(int *)b;
}

DataPoint::DataPoint(int _id, void *_datapoint) : id(_id), datapoint(_datapoint) {}

int DataPoint::getindex()
{
    return id;
}

void *DataPoint::getdata()
{
    return datapoint;
}

Vertex::Vertex(DataPoint *_data) : data(_data)
{
    NN = map_create(compare_ints, NULL, NULL);
    map_set_hash_function(NN, hash_int);
    map_set_destroy_key(NN, NULL);
    map_set_destroy_value(NN, NULL);

    RNN = map_create(compare_ints, NULL, NULL);
    map_set_hash_function(RNN, hash_int);
    map_set_destroy_key(RNN, NULL);
    map_set_destroy_value(RNN, NULL);
}

DataPoint *Vertex::getpointer()
{
    return data;
}

Map Vertex::getNeighbors()
{
    return NN;
}

Map Vertex::getReverseNeighbors()
{
    return RNN;
}

KNNGraph::KNNGraph(int _k, int size, void **dataset) : k(_k), dataset_size(size)
{
    vertexArray = new Vertex *[k];
    for (int i = 0; i < dataset_size; i++)
    {
        DataPoint d(i, dataset[i]); //???????????????????????????????????
        Vertex v(&d);
        vertexArray[i] = &v;
    }
}

KNNGraph::~KNNGraph()
{
    // for (int i = 0; i < dataset_size; ++i)
    // {
    //     delete vertexArray[i];
    // }
    delete[] vertexArray;
}

void *KNNGraph::get_vertex_info(int id)
{
    Vertex *v = vertexArray[id];
    DataPoint *dp = v->getpointer();
    return dp->getdata();
}