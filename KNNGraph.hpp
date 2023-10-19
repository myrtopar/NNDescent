#include <iostream>
#include <unordered_set>

using namespace std;

class DataPoint
{
private:
    int id;
    void *datapoint;

public:
    DataPoint(int _id, void *_datapoint) : id(_id), datapoint(_datapoint) {}
};

class Vertex
{
private:
    DataPoint *data;
    unordered_set<DataPoint *> Neighbors;
    unordered_set<DataPoint *> RNeighbors;
};

class KNNGraph
{
};
