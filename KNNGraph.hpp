#include <iostream>
#include <cstdlib> 
#include "ADTPriorityQueue.h"

using namespace std;

class Neighbor;

int compare_ints(Pointer a, Pointer b);

struct MyTuple {
    int num1;
    int num2;
    int num3;
};

// typedef double (*DistanceFunction)(const void* a, const void* b);

class DataPoint {
private:
    int id;
    void* datapoint;

public:
    DataPoint(int _id, void* _datapoint);
    
    int getId() const;
    void* getAddr() const;
};


class Vertex {
private:
    DataPoint* data;
    PriorityQueue NN;
    PriorityQueue RNN;

public:
    Vertex(DataPoint* _data);

    DataPoint *getData() const;
    void addNeighbor(Neighbor *neighbor);
    void addReverseNeighbor(Neighbor *neighbor);
    PriorityQueue getNeighbors() const;
    PriorityQueue getReverseNeighbors() const;
    //int findNeighbor(int id) const;
};

class Neighbor {
private:
    int *id;
    double *distance;
public:
    Neighbor(int _id, double _distance);
    int *getid();
    double *getDistance();
};


template <typename DataType, typename DistanceFunction>
class KNNGraph {
private:
    Vertex** vertexArray;
    int K;
    int size;
    DistanceFunction distance;

public:
    KNNGraph(int _K, int _size, DataType* myTuples, DistanceFunction _distance);

    void createRandomGraph(int K, Vertex **vertexArray);
    void printNeighbors() const;
    //void calculateKNN() const; 

    ~KNNGraph();
};


template <typename DataType, typename DistanceFunction>
class KNNGraphBruteForce {
private:
    Vertex** vertexArray;
    int K;
    int size;
    DistanceFunction distance;

public:
    KNNGraphBruteForce(int _K, int _size, DataType* myTuples, DistanceFunction _distance);

    void printNeighborsBF() const;
    void calculateKNNBF() const; 

    ~KNNGraphBruteForce();
};

template <typename DataType, typename DistanceFunction>
KNNGraphBruteForce<DataType, DistanceFunction>::KNNGraphBruteForce(int _K, int _size, DataType* myTuples, DistanceFunction _metricFunction) : K(_K), size(_size), distance(_metricFunction) {
    cout << "\nConstructing a BRUTEFORCE graph of " << size << " elements" << endl;
    vertexArray = new Vertex*[size];
    for (int i = 0; i < size; i++) {
        vertexArray[i] = new Vertex(new DataPoint(i, &myTuples[i]));
    }
    calculateKNNBF();
}



template <typename DataType, typename DistanceFunction>
void KNNGraphBruteForce<DataType, DistanceFunction>::calculateKNNBF() const {
    cout << "\nBrute Force on graph to create KNN..." << endl;

    for (int i = 0; i < size; i++) {
        Vertex* vertex = vertexArray[i];
        cout << "Vertex " << i << endl;
        for(int j= 0; j < size; j++){
            if(i == j)
                continue;

            cout << "Distance from vertex " << j;
            Vertex *nvertex = vertexArray[j];
            
            DataType* vertexData = static_cast<DataType*>(vertex->getData()->getAddr());
            DataType* neighborData = static_cast<DataType*>(nvertex->getData()->getAddr());
            
            // cout << "Vertex Data: (" << vertexData->num1 << ", " << vertexData->num2 << ", " << vertexData->num3 << ") ,";
            // cout << "  Neighbor Data: (" << neighborData->num1 << ", " << neighborData->num2 << ", " << neighborData->num3 << ")";

            double dist = distance(*vertexData, *neighborData);
            cout << ": " << dist << endl;

            Neighbor* newNeighbor = new Neighbor(j, dist);
            // cout << "adding neighbor no " << randomNeighborIndex << " in the nn PriorityQueue of " << i << endl;
            vertex->addNeighbor(newNeighbor);
            // cout << "adding reverse neighbor no " << i << " in the rnn PriorityQueue of " << randomNeighborIndex << endl;

        }
    
        cout << endl;
    }

    printNeighborsBF();

    for(int i = 0; i < size; i++){
        Vertex* vertex = vertexArray[i];
        for(int j = 0; j < size - K -1; j++){
            pqueue_remove_max(vertex->getNeighbors());
        }
    }
    printNeighborsBF();

}

template <typename DataType, typename DistanceFunction>
void KNNGraphBruteForce<DataType, DistanceFunction>::printNeighborsBF() const {
    cout << "\nPrinting Neighbors:" << endl;
    
    for (int i = 0; i < size; i++) {
        Vertex* vertex = vertexArray[i];
        cout << "Vertex " << i << " neighbors: ";
        PriorityQueue neighbors = vertex->getNeighbors();


        void **nArray = pqueue_to_array(neighbors);
        for (int j = 0; j < pqueue_size(neighbors); j++) {
            Neighbor *n = (Neighbor *)nArray[j];
            int *id = n->getid();
           cout << *id << " ";
        }
        free(nArray);
        
        
        cout << endl;
    }

    // cout << "\nPrinting Reverse Neighbors:" << endl;
    
    
}



template <typename DataType, typename DistanceFunction>
KNNGraphBruteForce<DataType, DistanceFunction>::~KNNGraphBruteForce() {
    for (int i = 0; i < size; i++) {
        delete vertexArray[i]; // Delete each Vertex
    }
    delete[] vertexArray; // Delete the array of Vertex pointers
}

// due to the template usage, the implementation of the functions below should be available in this file

template <typename DataType, typename DistanceFunction>
KNNGraph<DataType, DistanceFunction>::KNNGraph(int _K, int _size, DataType* myTuples, DistanceFunction _metricFunction) : K(_K), size(_size), distance(_metricFunction) {
    cout << "\nConstructing a graph of " << size << " elements" << endl;
    vertexArray = new Vertex*[size];
    for (int i = 0; i < size; i++) {
        vertexArray[i] = new Vertex(new DataPoint(i, &myTuples[i]));
    }
    createRandomGraph(K, vertexArray);
}


template <typename DataType, typename DistanceFunction>
void KNNGraph<DataType, DistanceFunction>::createRandomGraph(int K, Vertex **vertexArray) {
    cout << "\nInitializing the graph..." << endl;

    // Connect each vertex with K random neighbors
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < K; j++) {
            int randomNeighborIndex;
            do {
                randomNeighborIndex = rand() % size;
            } while (randomNeighborIndex == i);  // edw thelei allagh

            // } while (randomNeighborIndex == i || vertexArray[i]->findNeighbor(randomNeighborIndex) == 1);
            
            // calculate distance 
            DataType* vertexData = static_cast<DataType*>(vertexArray[i]->getData()->getAddr());
            DataType* neighborData = static_cast<DataType*>(vertexArray[randomNeighborIndex]->getData()->getAddr());
            
            cout << "Vertex Data: (" << vertexData->num1 << ", " << vertexData->num2 << ", " << vertexData->num3 << ") ,";
            cout << "  Neighbor Data: (" << neighborData->num1 << ", " << neighborData->num2 << ", " << neighborData->num3 << ")";

            double dist = distance(*vertexData, *neighborData);
            cout << "  -   Distance: " << dist << endl;

            Neighbor* newNeighbor = new Neighbor(randomNeighborIndex, dist);
            Neighbor* newReverseNeighbor = new Neighbor(i, dist);

            // cout << "adding neighbor no " << randomNeighborIndex << " in the nn PriorityQueue of " << i << endl;
            vertexArray[i]->addNeighbor(newNeighbor);
            // cout << "adding reverse neighbor no " << i << " in the rnn PriorityQueue of " << randomNeighborIndex << endl;
            vertexArray[randomNeighborIndex]->addReverseNeighbor(newReverseNeighbor);

        }

        cout << endl;

    }
    printf("Inserted all neighbors and reverse neighbors\n");
}



template <typename DataType, typename DistanceFunction>
void KNNGraph<DataType, DistanceFunction>::printNeighbors() const {
    cout << "\nPrinting Neighbors:" << endl;
    
    for (int i = 0; i < size; i++) {
        Vertex* vertex = vertexArray[i];
        cout << "Vertex " << i << " neighbors: ";
        PriorityQueue neighbors = vertex->getNeighbors();

        // for(int j = 1; j <= pqueue_size(neighbors); j++){
        //     void *n = pqueue_get_at(neighbors, j);
        //     cout << 
        // }

        void **nArray = pqueue_to_array(neighbors);
        for (int j = 0; j < pqueue_size(neighbors); j++) {
            Neighbor *n = (Neighbor *)nArray[j];
            int *id = n->getid();
           cout << *id << " ";
        }
        free(nArray);
        
        
        cout << endl;
    }

    // cout << "\nPrinting Reverse Neighbors:" << endl;
    
    
}


template <typename DataType, typename DistanceFunction>
KNNGraph<DataType, DistanceFunction>::~KNNGraph() {
    for (int i = 0; i < size; i++) {
        delete vertexArray[i]; // Delete each Vertex
    }
    delete[] vertexArray; // Delete the array of Vertex pointers
}


// template <typename DataType, typename DistanceFunction>
// void KNNGraph<DataType, DistanceFunction>::calculateKNN() const {
//     cout << "\nCalculate KNN..." << endl;

//     for (int i = 0; i < size; i++) {
//         Vertex* vertex = vertexArray[i];
//         cout << "Vertex " << i << " neighbors: ";
//         PriorityQueue neighbors = vertex->getNeighbors();
        
//         // Iterate through the neighbors 
//         PriorityQueueNode node = PriorityQueue_first(neighbors);
//         while (node != PriorityQueue_EOF) {
//             int *neighborId = (int *)PriorityQueue_node_key(neighbors, node);
//             cout << *neighborId << " ";

//             // PriorityQueueNode neighborNode = PriorityQueue_find_node(neighbors, neighborId);
//             // Neighbor nn = static_cast<Neighbor>(PriorityQueue_node_value(neighbors, neighborNode));
            
//             node = PriorityQueue_next(neighbors, node);
//         }
                
//         cout << endl;
//     }

// }