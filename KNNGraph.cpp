#include "KNNGraph.hpp"

KNNBruteForce::KNNBruteForce(int _K, int _size, int _dimensions, float** data, DistanceFunction _distance) : K(_K), size(_size), dimensions(_dimensions), distance(_distance)
{
    vertexArray = new Vertex *[size];
    for (int i = 0; i < size; i++)
    {
        vertexArray[i] = new Vertex(data[i]);
    }
    calculateKNNBF();
}

void KNNBruteForce::calculateKNNBF() const
{
    cout << "\nConstructing a graph of " << size << " elements, looking for " << K << " nearest neighbors" << endl;

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];

        for (int j = 0; j < size; j++)
        {
            if (i == j)
                continue;

            Vertex *nvertex = vertexArray[j];

            float *vertexData = static_cast<float *>(vertex->getData());
            float *neighborData = static_cast<float *>(nvertex->getData());

            double dist = distance(vertexData, neighborData, dimensions);

            Neighbor *newNeighbor = new Neighbor(j, dist);
            vertex->addNeighbor(newNeighbor);
        }
    }

    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        Set allNodes = vertex->getNeighbors();
        Set Kneighbors = set_create(compare_distances, delete_neighbor);

        int j = 0;
        for (SetNode node = set_first(allNodes); node != SET_EOF; node = set_next(allNodes, node))
        {
            if (j == K)
                break;
            Neighbor *p = (Neighbor *)set_node_value(allNodes, node);
            Neighbor *newNeighbor = new Neighbor(*p->getid(), *p->getDistance());
            set_insert(Kneighbors, newNeighbor);
            j++;
        }
        vertex->replaceNNSet(Kneighbors);
    }
}

// template <typename float, typename DistanceFunction>
int **KNNBruteForce::extract_neighbors_to_list()
{
    int **neighbors = new int *[size];
    for (int i = 0; i < size; i++)
    {
        neighbors[i] = new int[K];
    } // row col

    for (int i = 0; i < size; i++)
    {
        Vertex *v = vertexArray[i];
        Set nn = v->getNeighbors();

        int j = 0;
        for (SetNode node = set_first(nn); node != SET_EOF; node = set_next(nn, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(nn, node);
            neighbors[i][j] = *n->getid();
            j++;
        }
    }

    return neighbors;
}

// template <typename float, typename DistanceFunction>
void KNNBruteForce::printNeighbors()
{
    cout << "\033[1;31m BRUTE FORCE NEIGHBORS\033[0m" << endl;

    for (int i = 0; i < size; i++)
    {
        Set p = vertexArray[i]->getNeighbors();

        cout << "Neighbors of " << i << ": ";
        for (SetNode node = set_first(p); node != SET_EOF; node = set_next(p, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(p, node);
            cout << *n->getid() << "(" << n->getFlag() << ", " << *n->getDistance() << ") ";
        }
        cout << endl;
    }
}

// template <typename float, typename DistanceFunction>
KNNBruteForce::~KNNBruteForce()
{
    for (int i = 0; i < size; i++)
    {
        delete vertexArray[i];
    }
    delete[] vertexArray;
}

////////////////////////////////// KNNDESCENT //////////////////////////////////
// template <typename float, typename DistanceFunction>
KNNDescent::KNNDescent(int _K, int _size, float _sampling, int _dimensions, float **data, DistanceFunction _metricFunction, double _delta) : K(_K), size(_size), sampling(_sampling), dimensions(_dimensions), distance(_metricFunction), delta(_delta)
{
    cout << "\nConstructing a graph of " << size << " elements, looking for " << K << " nearest neighbors" << endl;
    vertexArray = new Vertex *[size];
    for (int i = 0; i < size; i++)
    {
        vertexArray[i] = new Vertex(data[i]);
    }
    createRandomGraph(K, vertexArray);
}

// template <typename float, typename DistanceFunction>
void KNNDescent::createRandomGraph(int K, Vertex **vertexArray)
{
    // Connect each vertex with K random neighbors
    for (int i = 0; i < size; i++)
    {
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

            int *randomId = create_int(randomNeighborIndex);
            set_insert(usedIds, randomId);

            // calculate distance
            float *vertexData = static_cast<float *>(vertexArray[i]->getData());
            float *neighborData = static_cast<float *>(vertexArray[randomNeighborIndex]->getData());

            double dist = distance(vertexData, neighborData, dimensions);

            Neighbor *newNeighbor = new Neighbor(randomNeighborIndex, dist);
            vertexArray[i]->addNeighbor(newNeighbor);

            Neighbor *newReverseNeighbor = new Neighbor(i, dist);
            vertexArray[randomNeighborIndex]->addReverseNeighbor(newReverseNeighbor);
        }
        set_destroy(usedIds);
    }
}

// template <typename float, typename DistanceFunction>
KNNDescent::~KNNDescent()
{
    for (int i = 0; i < size; i++)
    {
        delete vertexArray[i]; // Delete each Vertex
    }
    delete[] vertexArray; // Delete the array of Vertex pointers
}

// template <typename float, typename DistanceFunction>
void KNNDescent::calculatePotentialNewNeighbors4()
{
    // Every calculatePotentialNewNeighbors call calculates the potential neighbors of each vector and prepares the graph for updates.

    // for every vertex, we find its potential neighbors for update
    int count = 0;
    for (int i = 0; i < size; i++)
    {
        Vertex *vertex = vertexArray[i];
        Set neighbors = vertex->getNeighbors();
        Set reverseNeighbors = vertex->getReverseNeighbors();

        // UNION
        int union_size = set_size(neighbors) + set_size(reverseNeighbors);

        Neighbor *_id_union[union_size];
        memset(_id_union, 0, sizeof(_id_union));

        int m = 0;
        int added1 = 0;
        for (SetNode node = set_first(neighbors); node != SET_EOF; node = set_next(neighbors, node))
        {
            double current_rate = (double)added1 / (double)set_size(neighbors);
            if (current_rate > sampling)
                break;
            _id_union[m++] = (Neighbor *)set_node_value(neighbors, node);
            added1++;
        }

        int added2 = 0;
        for (SetNode node = set_first(reverseNeighbors); node != SET_EOF; node = set_next(reverseNeighbors, node))
        {
            double current_rate = (double)added2 / (double)set_size(reverseNeighbors);
            if (current_rate > sampling)
                break;
            _id_union[m++] = (Neighbor *)set_node_value(reverseNeighbors, node);
            added2++;
        }

        // local join in sets neighborArray and ReverseNeighborArray
        auto start1 = std::chrono::high_resolution_clock::now();

        for (int j = 0; j < union_size; j++)
        {
            Neighbor *n1 = _id_union[j];
            if (n1 == NULL)
                break;

            for (int k = 0; k < union_size; k++)
            {
                Neighbor *n2 = _id_union[k];
                if (n2 == NULL)
                    break;

                int id1 = *(n1->getid());
                int id2 = *(n2->getid());

                if (id1 == id2)
                {
                    continue;
                }

                if ((n1->getFlag() == 1) || (n2->getFlag() == 1))
                {
                    Vertex *v1 = vertexArray[id1];
                    Vertex *v2 = vertexArray[id2];

                    double dist;

                    float *data1 = static_cast<float *>(v1->getData());
                    float *data2 = static_cast<float *>(v2->getData());

                    dist = distance(data1, data2, dimensions);

                    Neighbor *furthest = furthest_neighbor(v1->getNeighbors());
                    if (dist < *(furthest->getDistance()))
                    {
                        Neighbor *newNeighbor = new Neighbor(id2, dist);
                        v1->addPotentialNeighbor(newNeighbor);
                    }
                }
            }
            n1->setFalse();
        }

        auto stop1 = std::chrono::high_resolution_clock::now();
        auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(stop1 - start1);
    }
}

// template <typename float, typename DistanceFunction>
int KNNDescent::updateGraph()
{
    int updated = 0;

    // for every vertex in the graph
    for (int i = 0; i < size; i++)
    {
        Set nn = vertexArray[i]->getNeighbors();
        Set pn = vertexArray[i]->getPotentialNeighbors();

        if (set_size(pn) == 0) // if there are no potential neighbors for update, move to the next vertex
            continue;

        Neighbor *closestPotential = closest_neighbor(pn);
        int closestPotentialId = *closestPotential->getid();
        double closestPotentialDistance = *closestPotential->getDistance();

        Neighbor *furthestNeighbor = furthest_neighbor(nn);
        int furthestNeighborId = *furthestNeighbor->getid();
        double furthestNeighborDistance = *furthestNeighbor->getDistance();

        // keep updating the neighbors while there is room for update: while there are potential neighbors that are closer to the node than the furthest current neighbor, do the update
        while (closestPotentialDistance < furthestNeighborDistance)
        {
            Neighbor *newNeighbor = new Neighbor(closestPotentialId, closestPotentialDistance);

            // placing the new neighbor in the set

            // if the potential neighbor we are about to insert is already a neighbor
            if (set_insert(nn, newNeighbor) == 0)
            {
                set_remove(pn, closestPotential); // we remove this potential (duplicate), and move on
                delete newNeighbor;

                if (set_size(pn) == 0)
                    break;

                closestPotential = closest_neighbor(pn);
                closestPotentialDistance = *closestPotential->getDistance();
                closestPotentialId = *closestPotential->getid();

                furthestNeighbor = furthest_neighbor(nn);
                furthestNeighborDistance = *furthestNeighbor->getDistance();
                furthestNeighborId = *furthestNeighbor->getid();
                continue;
            }
            updated++;

            set_remove(nn, furthestNeighbor); // removing the furthest one
            set_remove(pn, closestPotential); // updating the potential neighbor set

            // restore the reverse neighbors of the vertex
            Neighbor reverse_to_remove(i, furthestNeighborDistance);
            bool removed = set_remove(vertexArray[furthestNeighborId]->getReverseNeighbors(), &reverse_to_remove);

            Neighbor *new_reverse = new Neighbor(i, closestPotentialDistance);
            vertexArray[closestPotentialId]->addReverseNeighbor(new_reverse);

            if (set_size(pn) == 0)
                break;

            closestPotential = closest_neighbor(pn);
            closestPotentialDistance = *closestPotential->getDistance();
            closestPotentialId = *closestPotential->getid();

            furthestNeighbor = furthest_neighbor(nn);
            furthestNeighborDistance = *furthestNeighbor->getDistance();
            furthestNeighborId = *furthestNeighbor->getid();
        }
        vertexArray[i]->resetPNNSet();
    }
    double d = 0.01;
    double term = d * K * size;
    if (updated < term)
        return 0;
    return updated;
}

// template <typename float, typename DistanceFunction>
void KNNDescent::createKNNGraph()
{

    for (int i = 0; i < 10; i++)
    {
        cout << "\033[1;32miteration " << i << "\033[0m" << endl;
        calculatePotentialNewNeighbors4();
        int updates = updateGraph();
        // cout << "updates: " << updates << endl;
        if (updates == 0)
            break;
    }
}

// template <typename float, typename DistanceFunction>
int **KNNDescent::extract_neighbors_to_list()
{
    int **neighbors = new int *[size];
    for (int i = 0; i < size; i++)
    {
        neighbors[i] = new int[K];
    }

    for (int i = 0; i < size; i++)
    {
        Vertex *v = vertexArray[i];
        Set nn = v->getNeighbors();

        int j = 0;
        for (SetNode node = set_first(nn); node != SET_EOF; node = set_next(nn, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(nn, node);
            neighbors[i][j] = *n->getid();
            j++;
        }
    }

    return neighbors;
}

// template <typename float, typename DistanceFunction>
void **KNNDescent::NNSinglePoint(void *data)
{
    createKNNGraph();

    float *queryData = static_cast<float *>(data);
    void **nearest_neighbor_data_array;

    for (int i = 0; i < size; i++)
    {
        Vertex *v = vertexArray[i];
        float *vertexData = static_cast<float *>(v->getData());

        double dist = distance(vertexData, queryData, dimensions);
        if (dist == 0.0) // found the query data point
        {
            Set nn = v->getNeighbors();
            int neighbors_size = set_size(nn);
            nearest_neighbor_data_array = new void *[neighbors_size];
            int j = 0;
            for (SetNode node = set_first(nn); node != SET_EOF; node = set_next(nn, node))
            {
                Neighbor *n = (Neighbor *)set_node_value(nn, node);
                int neighbor_id = *n->getid();
                nearest_neighbor_data_array[i] = vertexArray[neighbor_id]->getData();
                j++;
            }
            break;
        }
    }

    return nearest_neighbor_data_array;
}

// template <typename float, typename DistanceFunction>
void KNNDescent::printPotential()
{
    cout << "\033[1;31mPOTENTIAL NEIGHBORS\033[0m" << endl;

    for (int i = 0; i < size; i++)
    {
        Set p = vertexArray[i]->getPotentialNeighbors();

        // int i = 0;
        cout << "Potential neighbors of " << i << ": ";
        for (SetNode node = set_first(p); node != SET_EOF; node = set_next(p, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(p, node);
            cout << *n->getid() << "(" << n->getFlag() << ", " << *n->getDistance() << ") ";
        }
        cout << endl;
    }
}

// template <typename float, typename DistanceFunction>
void KNNDescent::printNeighbors()
{
    cout << "\033[1;31mNEIGHBORS\033[0m" << endl;

    for (int i = 0; i < size; i++)
    {
        Set p = vertexArray[i]->getNeighbors();

        cout << "Neighbors of " << i << ": ";
        for (SetNode node = set_first(p); node != SET_EOF; node = set_next(p, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(p, node);
            cout << *n->getid() << "(" << n->getFlag() << ", " << *n->getDistance() << ") ";
        }
        cout << endl;
    }
}

// template <typename float, typename DistanceFunction>
void KNNDescent::printReverse()
{
    cout << "\033[1;31mREVERSE NEIGHBORS\033[0m" << endl;

    for (int i = 0; i < size; i++)
    {
        Set p = vertexArray[i]->getReverseNeighbors();

        cout << "Reverse neighbors of " << i << ": ";
        for (SetNode node = set_first(p); node != SET_EOF; node = set_next(p, node))
        {
            Neighbor *n = (Neighbor *)set_node_value(p, node);
            cout << *n->getid() << "(" << n->getFlag() << ", " << *n->getDistance() << ") ";
        }
        cout << endl;
    }
}
