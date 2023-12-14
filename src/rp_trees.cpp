#include "headers/KNNGraph.hpp"
#include "headers/rp_trees.hpp"

tree_node::tree_node(int _dimensions, float **_data, int N, int _limit) : dimensions(_dimensions), data(_data), numDataPoints(N), size_limit(_limit)
{
    leftChild = rightChild = NULL;
    hyperplaneVector = new float[100];
}

tree_node::~tree_node() {}

void tree_node::set_subtrees(TreeNode sub_left, TreeNode sub_right)
{
    leftChild = sub_left;
    rightChild = sub_right;
}

void tree_node::add_data(float *data_point)
{
    data[numDataPoints++] = data_point;
}

TreeNode tree_node::left_sub()
{
    return leftChild;
}

TreeNode tree_node::right_sub()
{
    return rightChild;
}

void tree_node::random_projection_split()
{

    float **left_sub_data = new float *[numDataPoints];
    float **right_sub_data = new float *[numDataPoints];

    TreeNode new_sub_left = new tree_node(dimensions, left_sub_data, 0, size_limit);
    TreeNode new_sub_right = new tree_node(dimensions, right_sub_data, 0, size_limit);

    set_subtrees(new_sub_left, new_sub_right);

    for (int i = 0; i < numDataPoints; i++)
    {
        double dotp = dot_product(data[i], hyperplaneVector, dimensions);

        if (dotp < 0)
        {
            new_sub_left->add_data(data[i]);
        }
        else if (dotp > 0)
        {
            new_sub_right->add_data(data[i]);
        }
        else if (dotp == 0)
        {
            (new_sub_left->numDataPoints <= new_sub_right->numDataPoints) ? new_sub_left->add_data(data[i]) : new_sub_right->add_data(data[i]);
        }
    }
}

void tree_node::rp_tree_rec()
{
    define_random_hyperplane(hyperplaneVector, dimensions, -0.4, 0.4);
    random_projection_split();
    if (leftChild->numDataPoints > size_limit)
    {
        leftChild->rp_tree_rec();
    }

    if (rightChild->numDataPoints > size_limit)
    {
        rightChild->rp_tree_rec();
    }
}

// other procedures
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

void define_random_hyperplane(float *hyperplaneVector, int dimensions, float min, float max)
{
    for (int i = 0; i < dimensions; i++)
    {
        hyperplaneVector[i] = generate_random_float(min, max);
    }
}