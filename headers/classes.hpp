#pragma once

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <chrono>
#include <cstring>
#include "ADTSet.h"

using namespace std;

class Neighbor;

float compare_ints(Pointer a, Pointer b);
float compare_distances(Pointer a, Pointer b);
int *create_int(int n);
void delete_data(float **data, uint32_t N);
void delete_int(void *a);
void delete_neighbor(void *a);
Neighbor *furthest_neighbor(Set s);
Neighbor *closest_neighbor(Set s);
double compare_results(int **array1, int **array2, int N, int K);
int contains(Neighbor *id_union[], int size, int targetId);
int random_int(int range, int coll);
float averageNeighborDistance(Set s);

class Vertex
{
private:
    void *datapoint;
    int id;
    Set NN;
    Set RNN;
    Set potentialNN;

public:
    Vertex(void *_data, int id);

    void *getData() const;
    int getId() const;
    int addNeighbor(Neighbor *neighbor);
    int addReverseNeighbor(Neighbor *neighbor);
    int addPotentialNeighbor(Neighbor *neighbor);

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
    float *distance;

public:
    Neighbor(int _id, float _distance);
    int *getid();
    float *getDistance();
    int getFlag();
    void setFalse();
    ~Neighbor();
};