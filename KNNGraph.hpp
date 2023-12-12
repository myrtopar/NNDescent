#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include "ADTSet.h"
#include "classes.hpp"

using namespace std;

class KNNDescent
{
private:
    int K;
    int size;
    float sampling;
    int dimensions;
    using DistanceFunction = float (*)(const float *, const float *, int);
    DistanceFunction distance;
    double delta;

public:
    KNNDescent(int _K, int _size, float _sampling, int dimensions, float **myTuples, DistanceFunction _distance, double delta);

    void createRandomGraph(int K, Vertex **vertexArray);
    void calculatePotentialNewNeighbors4(); // sampling version

    void printPotential();
    void printNeighbors();
    void printReverse();

    int updateGraph();
    void createKNNGraph();
    int **extract_neighbors_to_list();
    void **NNSinglePoint(void *data);

    ~KNNDescent();

    Vertex **vertexArray;
};

class KNNBruteForce
{
private:
    Vertex **vertexArray;
    int K;
    int size;
    int dimensions;
    using DistanceFunction = float (*)(const float *, const float *, int);
    DistanceFunction distance;

public:
    KNNBruteForce(int _K, int _size, int dimensions, float **data, DistanceFunction _distance);

    void calculateKNNBF() const;
    int **extract_neighbors_to_list();

    void printNeighbors();

    ~KNNBruteForce();
};

// KNNBruteForce::KNNBruteForce(int _K, int _size, int _dimensions, float** data, DistanceFunction _distance) : K(_K), size(_size), dimensions(_dimensions), distance(_distance);

// void KNNBruteForce::calculateKNNBF() const;

// int **KNNBruteForce::extract_neighbors_to_list();

// void KNNBruteForce::printNeighbors();

// KNNBruteForce::~KNNBruteForce();

// ////////////////////////////////// KNNDESCENT //////////////////////////////////
// KNNDescent::KNNDescent(int _K, int _size, float _sampling, int _dimensions, float **data, DistanceFunction _metricFunction, double _delta) : K(_K), size(_size), sampling(_sampling), dimensions(_dimensions), distance(_metricFunction), delta(_delta);

// void KNNDescent::createRandomGraph(int K, Vertex **vertexArray);

// KNNDescent::~KNNDescent();

// void KNNDescent::calculatePotentialNewNeighbors4();

// int KNNDescent::updateGraph();

// void KNNDescent::createKNNGraph();

// int **KNNDescent::extract_neighbors_to_list();

// void **KNNDescent::NNSinglePoint(void *data);

// void KNNDescent::printPotential();

// void KNNDescent::printNeighbors();

// void KNNDescent::printReverse();