#include "ADTSet.h"
#include "classes.hpp"
#include "rp_trees.hpp"

using namespace std;

using DistanceFunction = float (*)(const float*, const float*, int);
extern float **distanceResults;

class KNNDescent
{
private:
    int K;
    int size;
    float sampling;
    int dimensions;
    int rp_limit;
    using DistanceFunction = float (*)(const float *, const float *, int);
    DistanceFunction distance;
    double delta;

public:
    KNNDescent(int _K, int _size, float _sampling, int dimensions, float **data, DistanceFunction _distance, double delta, int rp_limit);

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
