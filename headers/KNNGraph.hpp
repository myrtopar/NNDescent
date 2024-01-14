#include "ADTSet.h"
#include "classes.hpp"
#include "rp_trees.hpp"

using namespace std;

using DistanceFunction = float (*)(int, int, const float *, const float *, int);
extern float *squares;

class KNNDescent
{
private:
    int K;
    int size;
    float **data;
    float sampling;
    int dimensions;
    int rp_limit;
    int num_trees;
    using DistanceFunction = float (*)(int, int, const float *, const float *, int);
    DistanceFunction distance;
    double delta;

    mutex *potentialNeighborsMutex;
    mutex *updateMutexes;
    mutex updateMutex;

public:
    Vertex **vertexArray;

    KNNDescent(int _K, int _size, float _sampling, int dimensions, float **data, DistanceFunction _distance, double delta);

    void createRandomGraph();
    void createRPGraph();
    void updateRPGraph();
    void updateRPGraphOpt();

    void calculatePotentialNewNeighbors();    // optimized version
    void calculatePotentialNewNeighborsOpt(); // parallelized version

    void parallelCalculatePotentialNewNeighbors(int start, int end);
    void parallelUpdate(int start, int end, int *updates);
    void parallelUpdateRPGraph(int start, int end);

    void printPotential();
    void printNeighbors();
    void printReverse();

    int updateGraph();
    int updateGraph2();
    void createKNNGraph();

    int **extract_neighbors_to_list();
    // void **NNSinglePoint(void *data);

    ~KNNDescent();
};