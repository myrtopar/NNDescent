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
double compare_distances(Pointer a, Pointer b);
int *create_int(int n);
void delete_data(float **data, uint32_t N);
void delete_int(void *a);
void delete_neighbor(void *a);

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

public:
    Vertex(DataPoint *_data);

    DataPoint *getData() const;
    void addNeighbor(Neighbor *neighbor);
    void addReverseNeighbor(Neighbor *neighbor);
    void addPotentialNeighbor(Neighbor *neighbor);
    Set getNeighbors() const;
    Set getReverseNeighbors() const;
    Set getPotentialNeighbors() const;
    Neighbor *furthest_neighbor(Set s);
    Neighbor *closest_neighbor(Set s);
    void replaceNNSet(Set set);
    void replaceRNNSet(Set set);
    void resetPNNSet();

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
