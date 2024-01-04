#include "ADTSet.h"
#include "classes.hpp"
#include "rp_trees.hpp"

using namespace std;

using DistanceFunction = float (*)(const float *, const float *, int);
extern float **distanceResults;

class KNNDescent
{
private:
    int K;
    int size;
    float **data;
    float sampling;
    int dimensions;
    int rp_limit;
    using DistanceFunction = float (*)(const float *, const float *, int);
    DistanceFunction distance;
    double delta;
    void parallelCalculatePotentialNewNeighbors(int start, int end);
    void parallelUpdate(int start, int end, int &updates);

    mutex *potentialNeighborsMutex;
    mutex* updateMutexes;
    mutex updateMutex;

public:
    Vertex **vertexArray;

    KNNDescent(int _K, int _size, float _sampling, int dimensions, float **data, DistanceFunction _distance, double delta, int rp_limit);

    void createRandomGraph();
    void createRPGraph();
    void calculatePotentialNewNeighbors4(); // sampling version
    void calculatePotentialNewNeighbors5(); // parallelized version

    void printPotential();
    void printNeighbors();
    void printReverse();

    int updateGraph();
    int updateGraph2();
    void createKNNGraph();

    int **extract_neighbors_to_list();
    void **NNSinglePoint(void *data);

    ~KNNDescent();
};