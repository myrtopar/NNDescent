#include "headers/KNNGraph.hpp"
#include "headers/rp_trees.hpp"

tree_node::tree_node(int _dimensions, float **_data, int N, int _limit) : dimensions(_dimensions), data(_data), numDataPoints(N), size_limit(_limit)
{
    leftChild = rightChild = NULL;
    hyperplaneVector = new float[100];
}

tree_node::~tree_node()
{
    delete[] hyperplaneVector;
    if (data != NULL)
        delete[] data;
}

void tree_node::delete_tree()
{
    delete_tree_recursive(this);
}

void delete_tree_recursive(TreeNode node)
{
    if (node == NULL)
    {
        return; // Base case: null pointer, nothing to delete
    }

    // Recursively delete left and right subtrees
    delete_tree_recursive(node->leftChild);
    delete_tree_recursive(node->rightChild);

    // Delete the current node
    delete node;

    // Optionally, set pointers to null to indicate deletion (optional)
    // node->left = nullptr;
    // node->right = nullptr;
}

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

float **tree_node::get_data()
{
    return data;
}

void tree_node::random_projection_split()
{
    float **left_sub_data = new float *[numDataPoints];
    float **right_sub_data = new float *[numDataPoints];

    for (int i = 0; i < numDataPoints; ++i)
    {
        left_sub_data[i] = nullptr;
        right_sub_data[i] = nullptr;
    }

    TreeNode new_sub_left = new tree_node(dimensions, left_sub_data, 0, size_limit);
    TreeNode new_sub_right = new tree_node(dimensions, right_sub_data, 0, size_limit);

    set_subtrees(new_sub_left, new_sub_right);

    double dotp = 0.0;

    for (int i = 0; i < numDataPoints; i++)
    {
        dotp = dot_product(data[i], hyperplaneVector, dimensions);

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

    // cout << "after split: datapoints on left: " << new_sub_left->numDataPoints << " , datapoints on right: " << new_sub_right->numDataPoints << endl;
}

void tree_node::rp_tree_rec(int *idx, TreeNode *leaf_array)
{
    // cout << "number of datapoints in this subtree: " << numDataPoints << endl;
    define_random_hyperplane(hyperplaneVector, dimensions, -0.4, 0.4);

    random_projection_split();
    if (leftChild->numDataPoints > size_limit)
    {
        leftChild->rp_tree_rec(idx, leaf_array);
    }
    else if (leftChild->numDataPoints <= size_limit && leftChild->numDataPoints > 0)
    {
        // cout << "storing a leaf in index " << *idx << endl;
        int curr_idx = *idx;
        leaf_array[curr_idx++] = leftChild;
        *idx = curr_idx;
    }

    if (rightChild->numDataPoints > size_limit)
    {
        rightChild->rp_tree_rec(idx, leaf_array);
    }
    else if (rightChild->numDataPoints <= size_limit && rightChild->numDataPoints > 0)
    {
        // cout << "storing a leaf in index " << *idx << endl;
        int curr_idx = *idx;
        leaf_array[curr_idx++] = rightChild;
        *idx = curr_idx;
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