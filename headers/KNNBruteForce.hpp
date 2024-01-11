#include "classes.hpp"

class KNNBruteForce
{
private:
    int K;
    int size;
    int dimensions;
    using DistanceFunction = float (*)(int, int, const float *, const float *, int);
    DistanceFunction distance;

public:
    Vertex **vertexArray;
    KNNBruteForce(int _K, int _size, int dimensions, float **data, DistanceFunction _distance);

    void calculateKNNBF() const;
    int **extract_neighbors_to_list();

    void printNeighbors();

    ~KNNBruteForce();
};