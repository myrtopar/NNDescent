#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include "ADTSet.h"
#include "classes.hpp"

using namespace std;

// due to the template usage, the implementation of the functions below should be available in this file

template <typename DataType, typename DistanceFunction>
class KNNDescent
{
private:
    int K;
    int size;
    int dimensions;
    DistanceFunction distance;

public:
    KNNDescent(int _K, int _size, int dimensions, DataType **myTuples, DistanceFunction _distance);

    void createRandomGraph(int K, Vertex **vertexArray);
    void printNeighbors() const;
    void printReverseNeighbors() const;
    void printPotentialNeighbors() const;
    void calculatePotentialNewNeighbors();
    void updateGraph();
    void createKNNGraph();

    void test_calculate_potential_new_neighbors();
    void test_update();
    ~KNNDescent();

    Vertex **vertexArray;
};

template <typename DataType, typename DistanceFunction>
class KNNBruteForce
{
private:
    Vertex **vertexArray;
    int K;
    int size;
    int dimensions;
    DistanceFunction distance;

public:
    KNNBruteForce(int _K, int _size, int dimensions, DataType **data, DistanceFunction _distance);

    void printNeighborsBF() const;
    void calculateKNNBF() const;

    ~KNNBruteForce();
};

///////////////////////////////////////////////////////////////////

template <typename DataType, typename DistanceFunction>
KNNBruteForce<DataType, DistanceFunction>::KNNBruteForce(int _K, int _size, int _dimensions, DataType **data, DistanceFunction _metricFunction) : K(_K), size(_size), dimensions(_dimensions), distance(_metricFunction)
{
    vertexArray = new Vertex *[size];
    for (int i = 0; i < size; i++)
    {
        vertexArray[i] = new Vertex(new DataPoint(i, data[i]));
    }
    calculateKNNBF();
}

template <typename DataType, typename DistanceFunction>
void KNNBruteForce<DataType, DistanceFunction>::calculateKNNBF() const
{
    cout << "\nBrute Force on graph to create KNN..." << endl;

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        // cout << "Vertex " << i << endl;

        for (int j = 0; j < size; j++)
        {
            if (i == j)
                continue;

            // cout << "Distance from vertex " << j;
            Vertex *nvertex = vertexArray[j];

            DataType *vertexData = static_cast<DataType *>(vertex->getData()->getAddr());
            DataType *neighborData = static_cast<DataType *>(nvertex->getData()->getAddr());

            double dist = distance(vertexData, neighborData, dimensions);
            // cout << ": " << dist << endl;

            Neighbor *newNeighbor = new Neighbor(j, dist);
            vertex->addNeighbor(newNeighbor);
        }
    }

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        // cout << "vertex " << i << endl;
        for (int j = 0; j < size - K - 1; j++)
        {
            SetNode s = set_last(vertex->getNeighbors());
            Neighbor *p = (Neighbor *)set_node_value(vertex->getNeighbors(), s);
            set_remove(vertex->getNeighbors(), p);
        }
    }

    printNeighborsBF();
}

template <typename DataType, typename DistanceFunction>
void KNNBruteForce<DataType, DistanceFunction>::printNeighborsBF() const
{
    cout << "\nPrinting Nearest Neighbors:" << endl;

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        cout << "Vertex " << i << " nearest neighbors: ";
        Set Neighbors = vertex->getNeighbors();

        int j = 0;
        for (SetNode node = set_first(Neighbors); node != SET_EOF; node = set_next(Neighbors, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(Neighbors, node);
            int *id = n->getid();
            double *dist = n->getDistance();
            cout << "\e[32m" << *id << "\e[0m"
                 << "(" << *dist << "), ";
        }
        cout << endl;
    }
}

template <typename DataType, typename DistanceFunction>
KNNBruteForce<DataType, DistanceFunction>::~KNNBruteForce()
{
    for (int i = 0; i < size; i++)
    {
        delete vertexArray[i]; // Delete each Vertex
    }
    delete[] vertexArray; // Delete the array of Vertex pointers
}

template <typename DataType, typename DistanceFunction>
KNNDescent<DataType, DistanceFunction>::KNNDescent(int _K, int _size, int _dimensions, DataType **data, DistanceFunction _metricFunction) : K(_K), size(_size), dimensions(_dimensions), distance(_metricFunction)
{
    cout << "\nConstructing a graph of " << size << " elements" << endl;
    vertexArray = new Vertex *[size];
    for (int i = 0; i < size; i++)
    {
        // cout << "data " << i << " address: " << data[i] << endl;
        vertexArray[i] = new Vertex(new DataPoint(i, data[i]));
    }
    createRandomGraph(K, vertexArray);
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::createRandomGraph(int K, Vertex **vertexArray)
{
    cout << "\nInitializing the graph..." << endl;

    // Connect each vertex with K random neighbors
    for (int i = 0; i < size; i++)
    {
        // cout << "Vertex " << i << endl;
        Set usedIds = set_create(compare_ints, delete_int);

        for (int j = 0; j < K; j++)
        {
            int randomNeighborIndex;
            do
            {
                randomNeighborIndex = rand() % size;
            } while (randomNeighborIndex == i || (set_find(usedIds, (void *)&randomNeighborIndex) != NULL));
            // if the random generated index is the vertex itself, we find another random neighbor
            // if the random generated index has already been chosen as one of the k random nearest neighbors previously, we find another random neighbor

            // cout << j << " random index: " << randomNeighborIndex << endl;
            int *randomId = create_int(randomNeighborIndex);
            set_insert(usedIds, randomId);

            // calculate distance
            DataType *vertexData = static_cast<DataType *>(vertexArray[i]->getData()->getAddr());
            DataType *neighborData = static_cast<DataType *>(vertexArray[randomNeighborIndex]->getData()->getAddr());

            double dist = distance(vertexData, neighborData, dimensions);

            Neighbor *newNeighbor = new Neighbor(randomNeighborIndex, dist);
            vertexArray[i]->addNeighbor(newNeighbor);

            Neighbor *newReverseNeighbor = new Neighbor(i, dist);
            vertexArray[randomNeighborIndex]->addReverseNeighbor(newReverseNeighbor);
        }
        set_destroy(usedIds);

        // cout << endl;
    }
    printf("Inserted all neighbors and reverse neighbors\n");
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::printNeighbors() const
{
    cout << "\nPrinting Nearest Neighbors:" << endl;

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        cout << "Vertex " << i << " nearest neighbors: ";
        Set Neighbors = vertex->getNeighbors();

        int j = 0;
        for (SetNode node = set_first(Neighbors); node != SET_EOF; node = set_next(Neighbors, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(Neighbors, node);
            int *id = n->getid();
            double *dist = n->getDistance();
            cout << "\e[1;32m" << *id << "\e[0m"
                 << "(" << *dist << "), ";
        }
        cout << endl;
    }
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::printReverseNeighbors() const
{
    cout << "\nPrinting Reverse Neighbors:" << endl;

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        cout << "Vertex " << i << " reverse neighbors: ";
        Set rNeighbors = vertex->getReverseNeighbors();

        int j = 0;
        for (SetNode node = set_first(rNeighbors); node != SET_EOF; node = set_next(rNeighbors, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(rNeighbors, node);
            int *id = n->getid();
            double *dist = n->getDistance();
            cout << "\e[1;32m" << *id << "\e[0m"
                 << "(" << *dist << "), ";
        }
        cout << endl;
    }
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::printPotentialNeighbors() const
{
    cout << "\nPrinting Potential Neighbors:" << endl;

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        cout << "Vertex " << i << " potential neighbors: ";
        Set pNeighbors = vertex->getPotentialNeighbors();

        int j = 0;
        for (SetNode node = set_first(pNeighbors); node != SET_EOF; node = set_next(pNeighbors, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(pNeighbors, node);
            int *id = n->getid();
            double *dist = n->getDistance();
            cout << "\e[1;32m" << *id << "\e[0m"
                 << "(" << *dist << "), ";
        }
        cout << endl;
    }
}

template <typename DataType, typename DistanceFunction>
KNNDescent<DataType, DistanceFunction>::~KNNDescent()
{
    for (int i = 0; i < size; i++)
    {
        delete vertexArray[i]; // Delete each Vertex
    }
    delete[] vertexArray; // Delete the array of Vertex pointers
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::calculatePotentialNewNeighbors()
{
    cout << "\nCalculate Potential New Neighbors..." << endl;
    // Every calculatePotentialNewNeighbors call calculates the potential neighbors of each vector and prepares the graph for updates.
    // Following the "friend of a friend" logic (local join between the NN and RNN sets), this call counts as one iteration through the vertices of the graph

    // for every vertex, we find its potential neighbors for update
    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i]; // node X
        // cout << "Vertex " << i << endl;

        Set neighbors = vertex->getNeighbors();
        Set reverseNeighbors = vertex->getReverseNeighbors();

        // UNION
        int arraysize = set_size(neighbors) + set_size(reverseNeighbors);
        int idArray[arraysize];

        int x = 0;
        for (SetNode node = set_first(neighbors); node != SET_EOF; node = set_next(neighbors, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(neighbors, node);
            int n_id = *(n->getid());
            idArray[x] = n_id;
            x++;
        }

        for (SetNode node = set_first(reverseNeighbors); node != SET_EOF; node = set_next(reverseNeighbors, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(reverseNeighbors, node);
            int rn_id = *(n->getid());
            idArray[x] = rn_id;
            x++;
        }

        // cout << "union of neighbors and reverse neighbors: ";
        // for (int l = 0; l < arraysize; l++)
        // {
        //     cout << idArray[l] << " ";
        // }
        // cout << endl;

        // local join in sets neighborArray and ReverseNeighborArray

        for (int j = 0; j < arraysize; j++) // A B C D E F
        {
            for (int k = 0; k < arraysize; k++) // A B C D E F
            {
                if (idArray[j] == idArray[k])
                    continue;

                int id1 = idArray[j]; // 6
                int id2 = idArray[k]; // 19
                // cout << "comparing " << id1 << " and " << id2;
                Vertex *v1 = vertexArray[id1];
                Vertex *v2 = vertexArray[id2];

                DataType *data1 = static_cast<DataType *>(v1->getData()->getAddr());
                DataType *data2 = static_cast<DataType *>(v2->getData()->getAddr());

                double dist = distance(data1, data2, dimensions);
                // cout << " - distance: " << dist << endl;
                // Neighbor *n = vertexArray[i]->closest_neighbor(neighbors);
                // cout << "adding to " << id1 << " potential neighbor " << id2 << " with distance " << dist << endl;
                Neighbor *newNeighbor = new Neighbor(id2, dist);
                if (set_find_node(vertexArray[id1]->getNeighbors(), newNeighbor) != NULL)
                {
                    // the pontential neighbor we are about to insert is already a neighbor of id1, so we skip this part
                    // cout << "vertex " << id1 << " already has " << id2 << " as a neighbor" << endl;
                    delete newNeighbor;
                }
                else
                {
                    v1->addPotentialNeighbor(newNeighbor);
                }
            }
        }

        // int j = 0;
        // for (SetNode node = set_first(neighbors); node != SET_EOF; node = set_next(neighbors, node))
        // {
        //     Neighbor *n = (Neighbor *)set_node_value(neighbors, node); // node A (A<-X)

        //     int n_id = *(n->getid());
        //     Vertex *v1 = vertexArray[n_id];
        //     // extracting the data from the neighbor vertex
        //     DataType *neighborData = static_cast<DataType *>(v1->getData()->getAddr());

        //     int k = 0;
        //     for (SetNode node = set_first(reverseNeighbors); node != SET_EOF; node = set_next(reverseNeighbors, node))
        //     {
        //         // cout << "reverse neighbor " << n_id;
        //         Neighbor *rn = (Neighbor *)set_node_value(reverseNeighbors, node); // node D (A<-X<-D)
        //         int rn_id = *(rn->getid());
        //         // cout << " and reverse neighbor " << rn_id;
        //         if (n_id == rn_id)
        //         {
        //             // cannot compare a vertex with itself, so we continue to the next reverse neighbor
        //             // cout << " have zero distance." << endl;
        //             k++;
        //             continue;
        //         }

        //         Vertex *v2 = vertexArray[rn_id];
        //         // extracting the data from the reverse neighbor vertex
        //         DataType *reverseNeighborData = static_cast<DataType *>(v2->getData()->getAddr());

        //         double dist = distance(neighborData, reverseNeighborData, dimensions);

        //         // cout << " have distance " << dist << endl;

        //         Neighbor *newPotentialNeighbor1 = new Neighbor(n_id, dist);  // add this to the pnn of the reverse
        //         Neighbor *newPotentialNeighbor2 = new Neighbor(rn_id, dist); // add this to the pnn of the neighbor

        //         if (set_find_node(vertexArray[rn_id]->getNeighbors(), newPotentialNeighbor1) != NULL)
        //         {
        //             // the pontential neighbor we are about to insert is already a neighbor of rn_id, so we skip this part
        //             // cout << "vertex " << rn_id << " already has " << n_id << " as a neighbor" << endl;
        //             delete newPotentialNeighbor1;
        //         }
        //         else
        //         {
        //             vertexArray[rn_id]->addPotentialNeighbor(newPotentialNeighbor1);
        //         }

        //         if (set_find_node(vertexArray[n_id]->getNeighbors(), newPotentialNeighbor2) != NULL)
        //         {
        //             // the pontential neighbor we are about to insert is already a neighbor of n_id, so we skip
        //             // cout << "vertex " << n_id << " already has " << rn_id << " as a neighbor" << endl;
        //             delete newPotentialNeighbor2;
        //         }
        //         else
        //         {
        //             vertexArray[n_id]->addPotentialNeighbor(newPotentialNeighbor2);
        //         }
        //     }
        // }

        // cout << endl;
    }
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::updateGraph()
{

    cout << "/////////////// UPDATING GRAPH //////////////" << endl;
    // for every vertex in the graph
    for (int i = 0; i < size; i++)
    {
        cout << "\nvertex " << i << " has potential new neighbors: ";

        //  will place the updated neighbors in this set and replace the old NN set with this one
        Set newNeighborSet = set_create(compare_distances, delete_neighbor);

        Set nn = vertexArray[i]->getNeighbors();
        Set pn = vertexArray[i]->getPotentialNeighbors();
        Set rn = vertexArray[i]->getReverseNeighbors();

        for (SetNode node = set_first(pn); node != SET_EOF; node = set_next(pn, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(nn, node);
            int nid = *n->getid();
            double ndist = *n->getDistance();
            cout << "\e[1;32m" << nid << "\e[0m"
                 << "(" << ndist << "), ";
        }
        cout << endl;

        // for every neighbor of the vertex
        int j = 0;
        for (SetNode node = set_first(nn); node != SET_EOF; node = set_next(nn, node))
        // for (SetNode node = set_last(nn); node != SET_EOF; node = set_previous(nn, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(nn, node);
            int nid = *n->getid();
            double ndist = *n->getDistance();

            Neighbor *p = vertexArray[i]->closest_neighbor(pn);

            if (p == NULL) // if there is no closest potential neighbor
            {
                // copy the neighbors from the old nn set to the new nn set
                // cout << "no potential neighbors for update, copying neighbor with id " << *n->getid() << " to the new set" << endl;
                Neighbor *newNeighbor = new Neighbor(nid, ndist);
                set_insert(newNeighborSet, newNeighbor);
                j++;
                continue;
            }

            int pid = *p->getid();
            double pdist = *p->getDistance();

            // cout << "neighbor with id: " << nid << " with distance " << ndist << " and potential neighbor with id: " << pid << " with distance " << pdist << ",";

            if (pdist < ndist)
            {

                // if the closest potential neighbor IS CLOSER than the existing neighbor, we make the potential neighbor a new one and add it to the new set
                Neighbor *newNeighbor = new Neighbor(pid, pdist);
                cout << "\e[34m replacing " << nid << " with " << pid << "\e[0m" << endl;
                set_insert(newNeighborSet, newNeighbor);
                set_remove(pn, p); // remove the potential neighbor because it was chosen for update

                // cout << "fixing reverse neighbors" << endl;
                //  fix reverse neighbor set
                //  remove the reverse neighbor that just got replaced
                Neighbor *to_remove = new Neighbor(i, ndist);
                // cout << "about to remove reverse neighbor " << i << " from reverse neighbors of " << nid << endl;
                set_remove(vertexArray[nid]->getReverseNeighbors(), to_remove);
                delete to_remove;

                // insert the new reverse neighbor that occured from the neighbor update
                // cout << "about to insert reverse neighbor " << i << " to the reverse neighbors of " << pid << endl;

                // extracting the data
                Vertex *v1 = vertexArray[pid];
                DataType *data1 = static_cast<DataType *>(v1->getData()->getAddr());

                Vertex *v2 = vertexArray[i];
                DataType *data2 = static_cast<DataType *>(v2->getData()->getAddr());

                double dist = distance(data1, data2, dimensions);

                Neighbor *newReverseNeighbor = new Neighbor(i, dist);
                vertexArray[pid]->addReverseNeighbor(newReverseNeighbor);
            }
            else
            {
                cout << "\e[35m not replacing " << nid << " with " << pid << "\e[0m" << endl;

                // copy the neighbor from the old nn set to the new nn set
                Neighbor *newNeighbor = new Neighbor(nid, ndist);
                set_insert(newNeighborSet, newNeighbor);
            }
            j++;
        }
        // cout << endl;
        vertexArray[i]->replaceNNSet(newNeighborSet);
        vertexArray[i]->resetPNNSet();
        if (set_size(vertexArray[i]->getPotentialNeighbors()) != 0)
        {
            cout << "\e[1;31mABORT\e[0m" << endl;
        }
    }
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::createKNNGraph()
{
    printNeighbors();
    // printReverseNeighbors();

    for (int i = 0; i < 7; i++)
    {
        calculatePotentialNewNeighbors();
        // printPotentialNeighbors();
        test_update();
        updateGraph();
        test_update();
        cout << endl
             << "after update " << i + 1 << endl;
        printNeighbors();
    }

    cout << "THE END" << endl;
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::test_update()
{
    int flag = 0;
    cout << "\nIN TEST UPDATE\n"
         << endl;

    int i;
    for (i = 0; i < size; i++)
    {
        Vertex *v = vertexArray[i];

        Set nn = v->getNeighbors();

        int j = 0;
        for (SetNode node = set_first(nn); node != SET_EOF; node = set_next(nn, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(nn, node);
            int id = *(n->getid());
            double dist1 = *(n->getDistance());
            Vertex *nv = vertexArray[id];

            DataType *data1 = static_cast<DataType *>(v->getData()->getAddr());
            DataType *data2 = static_cast<DataType *>(nv->getData()->getAddr());

            double dist2 = distance(data1, data2, dimensions);

            // cout << "vertex " << i << " neighbor " << id << endl;
            // cout << "distance from set: " << dist1 << ", distance from calculation: " << dist2 << endl;

            if (dist1 != dist2)
            {
                flag = 1;
                cout << "\e[31mERROR! VERTEX " << i << " AND NEIGHBOR VERTEX " << id << " HAVE CONFLICTING DISTANCES!\e[0m" << endl;
                cout << "set distance between " << i << " and " << id << ": " << dist1 << endl;
                cout << "real distance between " << i << " and " << id << ": " << dist2 << endl;
            }
        }
        // cout << endl;
    }

    for (i = 0; i < size; i++)
    {
        Vertex *v = vertexArray[i];

        Set rnn = v->getReverseNeighbors();

        int j = 0;
        for (SetNode node = set_first(rnn); node != SET_EOF; node = set_next(rnn, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(rnn, node);
            int id = *(n->getid());
            double dist1 = *(n->getDistance());
            Vertex *nv = vertexArray[id];

            DataType *data1 = static_cast<DataType *>(v->getData()->getAddr());
            DataType *data2 = static_cast<DataType *>(nv->getData()->getAddr());

            double dist2 = distance(data1, data2, dimensions);

            // cout << "vertex " << i << " neighbor " << id << endl;
            // cout << "distance from set: " << dist1 << ", distance from calculation: " << dist2 << endl;

            if (dist1 != dist2)
            {
                flag = 1;
                cout << "\e[31mERROR! VERTEX " << i << " AND REVERSE NEIGHBOR VERTEX " << id << " HAVE CONFLICTING DISTANCES!\e[0m" << endl;
                cout << "set distance between " << i << " and " << id << ": " << dist1 << endl;
                cout << "real distance between " << i << " and " << id << ": " << dist2 << endl;
            }
        }
        // cout << endl;
    }

    for (i = 0; i < size; i++)
    {
        Vertex *v = vertexArray[i];

        Set pnn = v->getPotentialNeighbors();

        int j = 0;
        for (SetNode node = set_first(pnn); node != SET_EOF; node = set_next(pnn, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(pnn, node);
            int id = *(n->getid());
            double dist1 = *(n->getDistance());
            Vertex *nv = vertexArray[id];

            DataType *data1 = static_cast<DataType *>(v->getData()->getAddr());
            DataType *data2 = static_cast<DataType *>(nv->getData()->getAddr());

            double dist2 = distance(data1, data2, dimensions);

            // cout << "vertex " << i << " neighbor " << id << endl;
            // cout << "distance from set: " << dist1 << ", distance from calculation: " << dist2 << endl;

            if (dist1 != dist2)
            {
                flag = 1;
                cout << "\e[31mERROR! VERTEX " << i << " AND POTENTIAL NEIGHBOR VERTEX " << id << " HAVE CONFLICTING DISTANCES!\e[0m" << endl;
                cout << "set distance between " << i << " and " << id << ": " << dist1 << endl;
                cout << "real distance between " << i << " and " << id << ": " << dist2 << endl;
            }
        }
        // cout << endl;
    }
    if (flag == 0)
    {
        cout << "\e[32mPassed the tests\e[0m" << endl;
    }
    else
    {
        cout << "\e[31mDid not pass the tests\e[0m" << endl;
    }
}

template <typename DataType, typename DistanceFunction>
void KNNDescent<DataType, DistanceFunction>::test_calculate_potential_new_neighbors()
{
}
