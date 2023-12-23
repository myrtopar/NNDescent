#pragma once

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <chrono>
#include <cstring> // For memset
#include "ADTSet.h"

using namespace std;

typedef class tree_node *TreeNode;

class tree_node
{
private:
    float **data;
    TreeNode leftChild;
    TreeNode rightChild;
    float *hyperplaneVector;

public:
    int size_limit;
    int dimensions;
    int numDataPoints;

    // function members
    tree_node(int dimensions, float **data, int N, int limit);
    void random_projection_split();
    void set_subtrees(TreeNode sub_left, TreeNode sub_right);
    void add_data(float *data_point);
    float **get_data();
    void rp_tree_rec(int *idx, TreeNode *leaf_array);
    TreeNode left_sub();
    TreeNode right_sub();

    // Destructor to free allocated memory
    ~tree_node();
};

double dot_product(const float *point1, const float *point2, int numDimensions);
int compare_doubles(double a, double b);
float generate_random_float(float min, float max);
void define_random_hyperplane(float *empty_vec, int dimensions, float min, float max);

// extra global variables