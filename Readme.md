**Documentation**
- **CONCEPT:**

Nearest neighbor search (NSS) is an optimization problem that aims to find the point in a given dataset that is closest to a specific data point, by measuring the distance using a metric function.
The project consists of 2 KNSS methods, Brute-Force and NN-Descent.


1. ***KNN_BruteForce*** class for K-Nearest Neighbors Search:
The KNNBruteForce method computes the K-nearest neighbors for every data point within the dataset by calculating the distance between itself and every other data point. We make the assumption that the datasets consist of float numbers. 

2. ***NN_Descent*** class for K-Nearest Neighbors Search:
KNNDescent is a graph-based algorithm for approximate k-Nearest Neighbor Search (KNN). It constructs a K-nearest neighbors graph (KNNG) to find K-nearest neighbors for each data point in a dataset. The algorithm begins by creating a random graph where each vertex is connected to K random neighbors. Then, it  updates the graph by computing potential neighbors based on their distance and adding them to the graph, so at the end it has the K-nearest neighbors for each vertex.
The calculatePotentialNewNeighbors method computes the potential neighbors for each vertex by considering both neighbors and reverse neighbors of a vertex, based on their distances. Specifically, for each pair of potential neighbor:
It calculates the distance between two vertices using the specified metric.
It checks if the calculated distance is greater than the distance to the furthest neighbor. 
If the distance is greater, the comparison is skipped, as the new potential neighbor is not closer than the current farthest neighbor.
Otherwise, a new potential neighbor is found to be closer so it adds it to the set of potential neighbors and updateGraph is being called .
UpdateGraph deletes the farthest neighbor, inserts the needed one and finally updates the reverse neigbors accordingly. This process is being repeated for each vertex, so that we ensure that the whole graph has been updated.



- **CLASSES:**
1. KNNDescent:
KNNDescent class keeps the number of neighbors to be found (K), the size of the dataset (size), the dimensions of the dataset (dimensions) and the distance metric between datapoints (distanceFunction). Lastly, it keeps an array of pointers to Vertex objects, where each Vertex represents a datapoint in the dataset (vertexArray). 

2. Vertex:
Vertex class keeps a pointer to the actual data (datapoint), and 3 sets - Nearest Neighbors of this vertex (NN), Reverse Nearest Neighbors of this vertex (RNN) and potential neighbors that might replace other neighbors of this vertex in the future (PN).
Finaly, it keeps a map that stores distances to other vertices (distances).

3. Neighbor:
Neighbor class keeps a pointer to an integer representing the ID of the neighbor vertex (id) and a pointer to a double representing the distance between the current vertex and this neighbor.

Some datasets are uploaded on this repository under the directory "datasets". The user's distance metric function
must be present on the file main.cpp. There is already an euclidean distance metric (for an arbitrary number of dimensions) in main.cpp.
The similarity between the brute force calculated graph and the NNDescent graph is calculated in the function compare_results. This function compares the edges of the 2 graphs based on the ids of the vertices and prints the similarity percentage. 
(Vertex id i refers to the pointer data[i] or i-th datapoint in file reading order)

In main, 2 calculations take place. First the brute force graph and then the NN-Descent graph, and later the similarity is calculated.

- **OPTIMIZATIONS:**

*Sampling:*

Due to the the high computational cost of local joins with a large K, we add a sampling strategy. This involves, marking nodes for comparison and constructing sampled lists of neighbors. User can adjust p (the second argument) to balance between precision and time.

*Graph Storing:*

One more optimazation in the k-nearest neighbors algorithm is that when the dataset size (N) is below or equal to 5000, the program uses a precomputed graph that we store in a file. This precomputed graph is retrieved and used to avoid redundant calculations.

*Distance Used: Euclidean with an optimization.*

We modified the Euclidean distance calculation by focusing on the squared distance. The absolute value of the distance was fount to be less significant in our case, emphasizing its comparison with other distances. This optimization, achieved through precomputing and storing vector norms using the binomial theorem in order to reduce the computational workload during distance calculations.

*Squares Precomputing:*

Squares of vector norms are used in the distance calculation later. This optimization saves considerable time, as the squares are computed only once at the beginning, reducing the computational workload during subsequent distance calculations. Additionally, we use the high-performance cblas_sdot function from the CBLAS library, ensuring very fast dot product calculations.

*Parallelization:*
4 methods got parallelized:
1. calculatePotentialNewNeighbors
2. pdateGraph
3. calculateSquares
4. updateRPGraphOpt

1. The calculatePotentialNewNeighborsOpt method is designed to take advantage of the available hardware concurrency by creating a number of threads (num_threads) dynamically based on the system's hardware capabilities. Each thread is then assigned a specific amount of the workload by making calls to the function parallelCalculatePotentialNewNeighbors.
calculatePotentialNewNeighborsOpt is a parallized version of calculatePotentialNewNeighbors. A mutex is used for each datapoint, strategically blocking access to shared data and preventing race conditions. 

2. The updateGraph2 method, has a similar threading pattern, but with a more complex locking
mechanism due to the amount of shared resources.

3. The calculateSquares method efficiently computes the squares of data points in a multi-dimensional space, using parallelization for better performance. This involves multiple threads working simultaneously on different subsets of data points.
The method dynamically decides the number of threads based on the hardware (num_threads). Each thread handles a specific range of data points, and the parallelSquares function is used concurrently to calculate the squares.
In the parallelSquares function, a mutex (squareMutex) is used to make sure only one thread accesses the shared squares array at a time. This ensures safe computation, preventing conflicts between threads and making the algorithm more reliable.

4. The parallelization of updateRPGraphOpt is the simplest one. It doesn't involve the use of mutexes, as shared resources are not at risk of concurrent access, but the concept is that the wordload only gets split in multiple threads to achieve time reduction.

- **DISCLAIMER:**

1. ADTSet is based on the open-source code of the k08 class (https://github.com/chatziko-k08/lecture-code).
2. cblas_sdot included in CBLAS library

- **HOW TO RUN:**

make run ARG='100 0.5 datasets/00001000-1.bin 0.01' : Runs the main function for K = 100, SamplingFactor = 0.5, DatasetPath = datasets/00001000-1.bin, Delta = 0.01
make test : Runs the tests

