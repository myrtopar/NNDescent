#include "KNNGraph.hpp"
#include "rp_trees.hpp"

TreeNode::TreeNode(int _dimensions, float **_data, int N) : data(_data), numDataPoints(N), leftChild(nullptr), rightChild(nullptr), dimensions(_dimensions)
{
    hyperplaneVector = new float[100];
}

TreeNode::~TreeNode() {}

void TreeNode::define_random_hyperplane(int dimensions, float min, float max)
{
    for (int i = 0; i < dimensions; i++)
    {
        hyperplaneVector[i] = generate_random_float(min, max);
    }
}

void TreeNode::random_projection_split()
{
    for (int i = 0; i < numDataPoints; i++)
    {
        double dotp = dot_product(data[i], hyperplaneVector, dimensions);
        // continue here
    }
}

double dot_product(const float *point1, const float *point2, int numDimensions)
{
    double prod = 0.0;
    for (int i = 0; i < numDimensions; i++)
    {
        prod += point1[i] * point2[i];
    }
    return prod;
}

int compare_doubles(double a, double b)
{
    return 10000 * a - 10000 * b;
}

float generate_random_float(float min, float max)
{
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}
