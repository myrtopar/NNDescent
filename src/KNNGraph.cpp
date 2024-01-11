#include "headers/KNNGraph.hpp"

////////////////////////////////// KNNDESCENT //////////////////////////////////
KNNDescent::KNNDescent(int _K, int _size, float _sampling, int _dimensions, float **_data, DistanceFunction _metricFunction, double _delta, int _rp_limit) : K(_K), size(_size), data(_data), sampling(_sampling), dimensions(_dimensions), distance(_metricFunction), delta(_delta), rp_limit(_rp_limit)
{
    cout << "\nConstructing a graph of " << size << " elements, looking for " << K << " nearest neighbors" << endl;
    vertexArray = new Vertex *[size];
    potentialNeighborsMutex = new mutex[size];
}

void KNNDescent::createRandomGraph()
{
    // assign each datapoint to a vertex
    for (int i = 0; i < size; i++)
    {
        vertexArray[i] = new Vertex(data[i], i);
    }

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

            // float dist = distanceResults[i][randomNeighborIndex];
            float dist = distance(vertexData, neighborData, dimensions);

            Neighbor *newNeighbor = new Neighbor(randomNeighborIndex, dist);
            vertexArray[i]->addNeighbor(newNeighbor);

            Neighbor *newReverseNeighbor = new Neighbor(i, dist);
            vertexArray[randomNeighborIndex]->addReverseNeighbor(newReverseNeighbor);
        }
        set_destroy(usedIds);
    }
}

void KNNDescent::createRPGraph()
{
    // assign each datapoint to a vertex
    for (int i = 0; i < size; i++)
    {
        vertexArray[i] = new Vertex(data[i], i);
    }

    TreeNode rp_root = new tree_node(dimensions, vertexArray, size, rp_limit);

    int expected_leaves = 0.5 * size;
    TreeNode *leaf_array = new TreeNode[expected_leaves];

    int *index = new int;
    *index = 0;

    rp_root->rp_tree_rec(index, leaf_array); // creating the rp tree recursively

    // for each leaf of the tree
    for (int i = 0; i < *index; i++)
    {
        Vertex **leaf_data = leaf_array[i]->get_data();
        int data_count = leaf_array[i]->numDataPoints;

        // for each datapoint of the cluster
        for (int j = 0; j < data_count; j++)
        {
            Vertex *v1 = leaf_data[j];
            float *d1 = static_cast<float *>(v1->getData());
            int v1_id = v1->getId();

            // firstly assign all the other datapoints of the cluster as its neighbors
            for (int k = 0; k < data_count; k++)
            {
                if (j == k)
                    continue;

                Vertex *v2 = leaf_data[k];
                float *d2 = static_cast<float *>(v2->getData());
                int v2_id = v2->getId();

                float dist = distance(d1, d2, dimensions); // pre-calculated??

                Neighbor *newNeighbor = new Neighbor(v2_id, dist);
                v1->addNeighbor(newNeighbor);

                Neighbor *newReverseNeighbor = new Neighbor(v1_id, dist);
                v2->addReverseNeighbor(newReverseNeighbor);
            }

            Set nn = v1->getNeighbors();
            // cout << "filled vertex " << v1_id << " with " << data_count << " cluster neighbors. Average neihbor distance: " << averageNeighborDistance(nn) << endl;

            // then fill in the remaining neighbors with random ones from the graph
            while (set_size(nn) < K)
            {
                // pick a random leaf
                int random_idx = random_int(*index, i);

                Vertex **random_leaf_data = leaf_array[random_idx]->get_data();

                // pick a random datapoint
                int data_idx = rand() % leaf_array[random_idx]->numDataPoints;
                Vertex *rv = random_leaf_data[data_idx];

                float *rd = static_cast<float *>(rv->getData());
                int rv_id = rv->getId();

                float dist = distance(d1, rd, dimensions); // also pre-calculated?

                Neighbor *newNeighbor = new Neighbor(rv_id, dist);
                if (!v1->addNeighbor(newNeighbor))
                {
                    continue;
                }

                Neighbor *newReverseNeighbor = new Neighbor(v1_id, dist);
                rv->addReverseNeighbor(newReverseNeighbor);
            }
        }
    }

    // creating an random projection forest in order to make the graph initialization more accurate

    rp_root->delete_tree();
    delete index;
    delete[] leaf_array;
}

void KNNDescent::updateRPGraph()
{
    TreeNode rp_root = new tree_node(dimensions, vertexArray, size, rp_limit);

    int expected_leaves = 0.5 * size;
    TreeNode *leaf_array = new TreeNode[expected_leaves];

    int *index = new int;
    *index = 0;

    rp_root->rp_tree_rec(index, leaf_array); // creating the rp tree recursively

    // for each leaf of the tree
    for (int i = 0; i < *index; i++)
    {
        // cluster data
        Vertex **leaf_data = leaf_array[i]->get_data();
        int data_count = leaf_array[i]->numDataPoints;

        // for each datapoint of the cluster
        for (int j = 0; j < data_count; j++)
        {
            Vertex *v1 = leaf_data[j];
            float *d1 = static_cast<float *>(v1->getData());
            int v1_id = v1->getId();

            // firstly assign all the other datapoints of the cluster as its neighbors
            for (int k = 0; k < data_count; k++)
            {
                if (j == k)
                    continue;

                Vertex *v2 = leaf_data[k];
                float *d2 = static_cast<float *>(v2->getData());
                int v2_id = v2->getId();

                float dist = distance(d1, d2, dimensions); // pre-calculated??

                Neighbor *furthest = furthest_neighbor(v1->getNeighbors());
                float furthestDistance = *(furthest->getDistance());
                int furtherstId = *(furthest->getid());

                // if this datapoint on this cluster is closer than at least one pre-assigned neighbor, update it
                if (dist < furthestDistance)
                {
                    Neighbor *newNeighbor = new Neighbor(v2_id, dist);

                    // if the neighbor is inserted successfully, remove the furthest neighbor to keep K neihbors
                    if (v1->addNeighbor(newNeighbor))
                    {
                        set_remove(v1->getNeighbors(), furthest);

                        // add the new reverse neighbor
                        Neighbor *newReverseNeighbor = new Neighbor(v1_id, dist);
                        v2->addReverseNeighbor(newReverseNeighbor);

                        // restore the reverse neighbors of the vertex
                        Neighbor reverse_to_remove(v1_id, furthestDistance);
                        bool removed = set_remove(vertexArray[furtherstId]->getReverseNeighbors(), &reverse_to_remove);
                    }
                }
            }
        }
    }

    rp_root->delete_tree();
    delete index;
    delete[] leaf_array;
}

KNNDescent::~KNNDescent()
{
    for (int i = 0; i < size; i++)
    {
        delete vertexArray[i]; // Delete each Vertex
    }
    delete[] vertexArray; // Delete the array of Vertex pointers
    delete[] potentialNeighborsMutex;
}

void KNNDescent::calculatePotentialNewNeighbors()
{
    // each calculatePotentialNewNeighbors call calculates the potential neighbors of each vector and prepares the graph for updates.

    // for each vertex, we find its potential neighbors for update
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

                    float *data1 = static_cast<float *>(v1->getData());
                    float *data2 = static_cast<float *>(v2->getData());

                    double dist = distanceResults[id1][id2];

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

void KNNDescent::parallelCalculatePotentialNewNeighbors(int start, int end)
{
    for (int i = start; i < end; ++i)
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

                lock_guard<std::mutex> lock(potentialNeighborsMutex[id1]);

                if (id1 == id2)
                {
                    continue;
                }

                if ((n1->getFlag() == 1) || (n2->getFlag() == 1))
                {
                    Vertex *v1 = vertexArray[id1];
                    Vertex *v2 = vertexArray[id2];

                    float *data1 = static_cast<float *>(v1->getData());
                    float *data2 = static_cast<float *>(v2->getData());

                    float dist = distanceResults[id1][id2];

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

void KNNDescent::calculatePotentialNewNeighborsOpt()
{
    const int num_threads = std::thread::hardware_concurrency();
    thread threads[num_threads];

    int chunk_size = size / num_threads;
    int remaining = size % num_threads;
    int start = 0;
    int end = 0;

    for (int i = 0; i < num_threads; ++i)
    {
        end = start + chunk_size + (i < remaining ? 1 : 0);
        threads[i] = thread(&KNNDescent::parallelCalculatePotentialNewNeighbors, this, start, end);
        start = end;
    }

    // wait for all threads to finish
    for (int i = 0; i < num_threads; ++i)
    {
        threads[i].join();
    }
}

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
        float closestPotentialDistance = *closestPotential->getDistance();

        Neighbor *furthestNeighbor = furthest_neighbor(nn);
        int furthestNeighborId = *furthestNeighbor->getid();
        float furthestNeighborDistance = *furthestNeighbor->getDistance();

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
    double term = delta * K * size;
    if (updated < term)
        return 0;
    return updated;
}

void KNNDescent::parallelUpdate(int start, int end, int *updated)
{

    for (int i = start; i < end; i++)
    {
        std::unique_lock<std::mutex> lock1(vertexArray[i]->getUpdateMutex(), std::defer_lock);

        lock1.lock();

        Set nn = vertexArray[i]->getNeighbors();
        Set pn = vertexArray[i]->getPotentialNeighbors();

        if (set_size(pn) == 0)
        { // if there are no potential neighbors for update, move to the next vertex
            lock1.unlock();
            continue;
        }

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
                {
                    break;
                }

                closestPotential = closest_neighbor(pn);
                closestPotentialDistance = *closestPotential->getDistance();
                closestPotentialId = *closestPotential->getid();

                furthestNeighbor = furthest_neighbor(nn);
                furthestNeighborDistance = *furthestNeighbor->getDistance();
                furthestNeighborId = *furthestNeighbor->getid();
                continue;
            }
            // std::unique_lock<std::mutex> lockupdate(updateMutex);
            (*updated)++;
            // lockupdate.unlock();
            set_remove(nn, furthestNeighbor); // removing the furthest one
            set_remove(pn, closestPotential); // updating the potential neighbor set

            // restore the reverse neighbors of the vertex
            Neighbor reverse_to_remove(i, furthestNeighborDistance);

            std::unique_lock<std::mutex> lock2(vertexArray[furthestNeighborId]->getUpdateMutex(), std::defer_lock);
            std::unique_lock<std::mutex> lock3(vertexArray[closestPotentialId]->getUpdateMutex(), std::defer_lock);

            while (std::try_lock(lock2, lock3) != -1)
            {
                lock1.unlock();
                lock1.lock();
            }

            bool removed = set_remove(vertexArray[furthestNeighborId]->getReverseNeighbors(), &reverse_to_remove);
            lock2.unlock();

            Neighbor *new_reverse = new Neighbor(i, closestPotentialDistance);
            vertexArray[closestPotentialId]->addReverseNeighbor(new_reverse);
            lock3.unlock();

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
        lock1.unlock();
    }

    cout << "THE END //////////////////////////////////////////////////////////////////////" << start << endl;
}

int KNNDescent::updateGraph2()
{
    int updated = 0;
    double term = delta * K * size;

    const int num_threads = std::thread::hardware_concurrency();
    thread threads[num_threads];

    int *updates[num_threads];
    for (int i = 0; i < num_threads; i++)
    {
        updates[i] = new int;
        *updates[i] = 0;
    }

    int chunk_size = size / num_threads;
    int remaining = size % num_threads;
    int start = 0;
    int end = 0;

    for (int i = 0; i < num_threads; ++i)
    {
        end = start + chunk_size + (i < remaining ? 1 : 0);
        threads[i] = thread(&KNNDescent::parallelUpdate, this, start, end, updates[i]);
        start = end;
    }

    // wait for all threads to finish
    for (int i = 0; i < num_threads; ++i)
    {
        threads[i].join();
    }

    for (int i = 0; i < num_threads; i++)
    {
        updated += *(updates[i]);
        delete updates[i];
    }

    if (updated < term)
        return 0;
    return updated;
}

void KNNDescent::createKNNGraph()
{
    createRandomGraph();

    for (int i = 0; i < 10; i++)
    {
        cout << "\033[1;32miteration " << i << "\033[0m" << endl;
        calculatePotentialNewNeighborsOpt();

        auto start1 = std::chrono::high_resolution_clock::now();
        int updates = updateGraph2();
        auto stop1 = std::chrono::high_resolution_clock::now();
        auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(stop1 - start1);
        cout << "Time taken: " << duration1.count() << " microseconds\n";

        // cout << "updates: " << updates << endl;
        if (updates == 0)
            break;
    }
}

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

void **KNNDescent::NNSinglePoint(void *data)
{
    createKNNGraph();

    float *queryData = static_cast<float *>(data);
    void **nearest_neighbor_data_array;

    for (int i = 0; i < size; i++)
    {
        Vertex *v = vertexArray[i];
        float *vertexData = static_cast<float *>(v->getData());

        float dist = distance(vertexData, queryData, dimensions);
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
            cout << *n->getid() << /*"(" << n->getFlag() << ", " << *n->getDistance() <<*/ " ";
        }
        cout << endl;
    }
}

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
            cout << *n->getid() << /*"(" << n->getFlag() << ", " << *n->getDistance() <<*/ " ";
        }
        cout << endl;
    }
}