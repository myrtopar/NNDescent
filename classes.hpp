#pragma once

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <chrono>
#include "ADTSet.h"
<<<<<<< HEAD
#include "ADTMap.h"
=======
>>>>>>> ebdf46bbcb35040e5ef0c96143b4685392b4a7c8

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

void compare_results(int **array1, int **array2, int N, int K);

class Vertex
{
private:
    void *datapoint;
    Set NN;
    Set RNN;
    Set potentialNN;
<<<<<<< HEAD
    Map distances;
=======
>>>>>>> ebdf46bbcb35040e5ef0c96143b4685392b4a7c8

public:
    Vertex(void *_data);

    void *getData() const;
    void addNeighbor(Neighbor *neighbor);
    void addReverseNeighbor(Neighbor *neighbor);
    void addPotentialNeighbor(Neighbor *neighbor);
<<<<<<< HEAD
    void addDistance(int id, double dist);
=======
>>>>>>> ebdf46bbcb35040e5ef0c96143b4685392b4a7c8

    Set getNeighbors() const;
    Set getReverseNeighbors() const;
    Set getPotentialNeighbors() const;
<<<<<<< HEAD
    Map getDistances() const;

    void replaceNNSet(Set set);
    void replaceRNNSet(Set set);
    void resetPNNSet();
    void resetRNNSet();
=======

    void replaceNNSet(Set set);
    void resetPNNSet();
>>>>>>> ebdf46bbcb35040e5ef0c96143b4685392b4a7c8

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
