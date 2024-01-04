#include "headers/KNNBruteForce.hpp"

////////////////////////////////// KNN BRUTE FORCE //////////////////////////////////

KNNBruteForce::KNNBruteForce(int _K, int _size, int _dimensions, float **data, DistanceFunction _distance) : K(_K), size(_size), dimensions(_dimensions), distance(_distance)
{
    vertexArray = new Vertex *[size];
    for (int i = 0; i < size; i++)
    {
        vertexArray[i] = new Vertex(data[i], i);
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

            // float dist = distanceResults[i][j];
            float dist = distance(vertexData, neighborData, dimensions);

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

KNNBruteForce::~KNNBruteForce()
{
    for (int i = 0; i < size; i++)
    {
        delete vertexArray[i];
    }
    delete[] vertexArray;
}