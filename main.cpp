#include <iostream>
#include "ADTMap.h"
#include "KNNGraph.hpp"

using namespace std;

int main()
{
    int d1[3] = {1, 2, 3};
    int d2[3] = {4, 5, 6};
    int d3[3] = {7, 8, 9};
    int d4[3] = {10, 11, 12};
    int d5[3] = {13, 14, 15};
    int d6[3] = {16, 17, 18};
    int d7[3] = {19, 20, 21};
    int d8[3] = {22, 23, 24};

    void *dataArray[8]; // Array of void pointers

    dataArray[0] = d1;
    dataArray[1] = d2;
    dataArray[2] = d3;
    dataArray[3] = d4;
    dataArray[4] = d5;
    dataArray[5] = d6;
    dataArray[6] = d7;
    dataArray[7] = d8;

    // Pointer data = myArray;
    // DataPoint sample(0, data);
    // Vertex vert(&sample);
    // Map nn = vert.getNeighbors();
    // cout << "neighbors size: " << map_size(nn) << endl;

    KNNGraph G(3, 8, dataArray);
    void *randomdp = G.get_vertex_info(2);
    // den prolava na elegksw tin orthotita olwn aytwn pou exw grapsei
}