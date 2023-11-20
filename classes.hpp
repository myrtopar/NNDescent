#pragma once

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <chrono>
#include "ADTSet.h"

using namespace std;

class Neighbor;

double compare_ints(Pointer a, Pointer b);
double compare_distances(Pointer a, Pointer b);
int *create_int(int n);
void delete_data(float **data, uint32_t N);
void delete_int(void *a);
void delete_neighbor(void *a);
Neighbor *furthest_neighbor(Set s);
Neighbor *closest_neighbor(Set s);

double compare_results(int **array1, int **array2, int N, int K);

class Vertex
{
private:
    void *datapoint;
    Set NN;
    Set RNN;
    Set potentialNN;

public:
    Vertex(void *_data);

    void *getData() const;
    void addNeighbor(Neighbor *neighbor);
    void addReverseNeighbor(Neighbor *neighbor);
    void addPotentialNeighbor(Neighbor *neighbor);

    Set getNeighbors() const;
    Set getReverseNeighbors() const;
    Set getPotentialNeighbors() const;

    void replaceNNSet(Set set);
    void resetPNNSet();

    ~Vertex();
};

class Neighbor
{
private:
    int *id;
    int flag;
    double *distance;

public:
    Neighbor(int _id, double _distance);
    int *getid();
    double *getDistance();
    int getFlag();
    void setFalse();
    ~Neighbor();
};
