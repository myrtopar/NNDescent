#pragma once

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <cblas.h>
#include <string>
#include <chrono>
#include <cstring> // For memset
#include "ADTSet.h"

using namespace std;

typedef class tree_node *TreeNode;

class tree_node
{
private:
    Vertex **data;
    float *hyperplaneVector;

public:
    int size_limit;
    int dimensions;
    int numDataPoints;
    TreeNode leftChild;
    TreeNode rightChild;

    // function members
    tree_node(int dimensions, Vertex **data, int N, int limit);
    void random_projection_split();
    void set_subtrees(TreeNode sub_left, TreeNode sub_right);
    void add_data(Vertex *data_point);
    Vertex **get_data();
    void rp_tree_rec(int *idx, TreeNode *leaf_array);
    TreeNode left_sub();
    TreeNode right_sub();
    void delete_tree();

    // Destructor to free allocated memory
    ~tree_node();
};

double dot_product(const float *point1, const float *point2, int numDimensions);
int compare_doubles(double a, double b);
float generate_random_float(float min, float max);
void define_random_hyperplane(float *empty_vec, int dimensions, float min, float max);
void delete_tree_recursive(TreeNode root);
