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

class TreeNode
{
private:
    float **data;
    int numDataPoints;
    int dimensions;
    TreeNode *leftChild;
    TreeNode *rightChild;
    float *hyperplaneVector;

public:
    // Constructor
    TreeNode(int dimensions, float **data, int N);
    void define_random_hyperplane(int dimensions, float min, float max);
    void random_projection_split();

    // Destructor to free allocated memory
    ~TreeNode();
};

double dot_product(const float *point1, const float *point2, int numDimensions);
int compare_doubles(double a, double b);
float generate_random_float(float min, float max);
