#pragma once

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include "ADTSet.h"

using namespace std;

class Neighbor;

double compare_ints(Pointer a, Pointer b);
int *create_int(int n);
void delete_data(float **data, uint32_t N);

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
    Set usedIds;

public:
    Vertex(DataPoint *_data);

    DataPoint *getData() const;
    void addNeighbor(Neighbor *neighbor);
    void addReverseNeighbor(Neighbor *neighbor);
    void addPotentialNeighbor(Neighbor *neighbor);
    void addUsedId(int *id);
    int findNeighbor(int id);
    Set getNeighbors() const;
    Set getReverseNeighbors() const;
    Set getPotentialNeighbors() const;
    Set getUsedIds() const;
    ~Vertex();
};

class Neighbor
{
private:
    int *id;
    double *distance;

public:
    Neighbor(int _id, double _distance);
    int *getid();
    double *getDistance();
    ~Neighbor();
};
