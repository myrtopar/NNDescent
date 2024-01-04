#include "headers/acutest.h"
#include "headers/KNNGraph.hpp"
#include "headers/KNNBruteForce.hpp"

const char *file_path = "datasets/00000200.bin";
int N;
const int num_dimensions = 100;
float **data;
int K;
float p = 0.5;

float **distanceResults;

bool set_is_proper(Set set);

typedef float (*DistanceFunction)(const float *, const float *, int);
float calculateEuclideanDistance(const float *point1, const float *point2, int numDimensions)
{
    double sum = 0.0;
    for (int i = 0; i < numDimensions; i++)
    {
        double diff = point1[i] - point2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

void calculateALLdistances(float **data, int N, int num_dimensions)
{
    distanceResults = new float *[N * N];
    for (int i = 0; i < N; i++)
    {
        distanceResults[i] = new float[N];
    }

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            distanceResults[i][j] = calculateEuclideanDistance(data[i], data[j], num_dimensions);
        }
    }
}

// Έλεγχος της insert σε λιγότερο χώρο
void insert_and_test(Set set, Pointer value)
{
    set_insert(set, value);
    TEST_ASSERT(set_is_proper(set));
    // TEST_ASSERT(set_find(set, value) == value);
}

// Βοηθητική συνάρτηση για το ανακάτεμα του πίνακα τιμών
void shuffle(int **array, int n)
{
    for (int i = 0; i < n; i++)
    {
        int j = i + rand() / (RAND_MAX / (n - i) + 1);
        int *t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

double calculateManhattanDistance(const float *point1, const float *point2, int numDimensions)
{
    double sum = 0.0;
    for (int i = 0; i < numDimensions; i++)
    {
        sum += fabs(point1[i] - point2[i]);
    }
    return sum;
}

void start_program()
{
    cout << "\nReading Data: " << file_path << endl;

    ifstream ifs;
    ifs.open(file_path, ios::binary);
    if (!ifs.is_open())
    {
        cout << "Failed to open the file." << endl;
        TEST_ASSERT(0);
        return;
    }

    // Read the number of points (N) and store it in the global variable
    ifs.read((char *)&N, sizeof(int));
    cout << "# of points: " << N << endl;

    const int num_dimensions = 100;

    // Create arrays for storing the data
    data = new float *[N];
    for (int i = 0; i < N; i++)
    {
        data[i] = new float[num_dimensions];
    }

    for (int i = 0; i < N; i++)
    {
        for (int d = 0; d < num_dimensions; d++)
        {
            float value;
            ifs.read((char *)(&value), sizeof(float));
            data[i][d] = value;
        }
    }

    ifs.close();
    calculateALLdistances(data, N, num_dimensions);
}

void end_program(void)
{
    // Clean up data arrays
    delete_data(data, N);
}

// Tests if the neighbor sets hold the correct distaces
void test_distances(void)
{
    int delta = 0.01;
    start_program();
    DistanceFunction distanceFunction = &calculateEuclideanDistance;

    KNNDescent KNNGraph(10, N, p, num_dimensions, data, distanceFunction, delta, 10);

    Vertex **array = KNNGraph.vertexArray;

    for (int r = 0; r < 10; r++)
    {
        KNNGraph.calculatePotentialNewNeighbors4();

        // array to store distances for each node
        double **nodeDistancesNN = new double *[N];
        double **nodeDistancesRN = new double *[N];
        double **nodeDistancesPN = new double *[N];

        for (int i = 0; i < N; i++)
        {
            Vertex *v = array[i];
            Set nn = v->getNeighbors();
            Set rn = v->getReverseNeighbors();
            Set pn = v->getPotentialNeighbors();

            nodeDistancesNN[i] = new double[set_size(nn)];
            nodeDistancesRN[i] = new double[set_size(rn)];
            nodeDistancesPN[i] = new double[set_size(pn)];

            int j = 0;
            for (SetNode node = set_first(nn); node != SET_EOF; node = set_next(nn, node))
            {
                Neighbor *n = (Neighbor *)set_node_value(nn, node);
                int *id = n->getid();
                float *dist = n->getDistance();
                float exactDistance = calculateEuclideanDistance(data[i], data[*id], num_dimensions);
                nodeDistancesNN[i][j] = *dist;

                TEST_ASSERT((exactDistance - *dist) == 0);
                j++;
            }

            j = 0;
            for (SetNode node = set_first(rn); node != SET_EOF; node = set_next(rn, node))
            {
                Neighbor *n = (Neighbor *)set_node_value(rn, node);
                int *id = n->getid();
                float *dist = n->getDistance();
                float exactDistance = calculateEuclideanDistance(data[i], data[*id], num_dimensions);
                nodeDistancesRN[i][j] = *dist;

                TEST_ASSERT((exactDistance - *dist) == 0);
                j++;
            }

            j = 0;
            for (SetNode node = set_first(pn); node != SET_EOF; node = set_next(pn, node))
            {
                Neighbor *n = (Neighbor *)set_node_value(pn, node);
                int *id = n->getid();
                float *dist = n->getDistance();
                float exactDistance = calculateEuclideanDistance(data[i], data[*id], num_dimensions);
                nodeDistancesPN[i][j] = *dist;
                TEST_ASSERT((exactDistance - *dist) == 0);
                j++;
            }
        }

        for (int i = 0; i < N; i++)
        {
            delete[] nodeDistancesNN[i];
            delete[] nodeDistancesRN[i];
            delete[] nodeDistancesPN[i];
        }

        delete[] nodeDistancesNN;
        delete[] nodeDistancesRN;
        delete[] nodeDistancesPN;

        if (KNNGraph.updateGraph() == 0)
            break;
    }

    end_program();
}

// Test if the potential neighbors set has been cleaned up, after updateGraph has been called
void test_potential()
{
    int delta = 0.01;
    start_program();

    DistanceFunction distanceFunction = &calculateEuclideanDistance;
    KNNDescent KNNGraph(10, N, p, num_dimensions, data, distanceFunction, delta, 10);

    for (int i = 0; i < 10; i++)
    {
        KNNGraph.calculatePotentialNewNeighbors4();
        if (KNNGraph.updateGraph() == 0)
            break;

        for (int j = 0; j < N; j++)
        {
            Vertex *vertex = KNNGraph.vertexArray[j];
            TEST_ASSERT((set_size(vertex->getPotentialNeighbors()) == 0));
        }
    }

    end_program();
}

void test_result()
{
    start_program();

    int K = 40;
    double delta = 0.01;
    DistanceFunction distanceFunction = &calculateEuclideanDistance;

    // knn descent method
    auto start1 = std::chrono::high_resolution_clock::now();
    KNNDescent KNNGraph(K, N, p, num_dimensions, data, distanceFunction, delta, 10);
    KNNGraph.createKNNGraph();
    auto stop1 = std::chrono::high_resolution_clock::now();

    auto duration1 = std::chrono::duration_cast<std::chrono::seconds>(stop1 - start1);
    cout << "KNNDescent: " << duration1.count() << " seconds" << endl;

    // brute force method
    auto start2 = std::chrono::high_resolution_clock::now();
    KNNBruteForce myGraph(K, N, num_dimensions, data, distanceFunction);
    auto stop2 = std::chrono::high_resolution_clock::now();

    auto duration2 = std::chrono::duration_cast<std::chrono::seconds>(stop2 - start2);
    cout << "Brute Force: " << duration2.count() << " seconds" << endl;

    // extract knn descent and brute force method results into a list and compare them, to find similarity percentage
    int **NND = KNNGraph.extract_neighbors_to_list();
    int **BF = myGraph.extract_neighbors_to_list();

    int count = 0;
    for (int i = 0; i < (int)N; i++)
    {
        for (int j = 0; j < K; j++)
        {
            int found = 0;
            for (int k = 0; k < K; k++)
            {
                if (NND[i][j] == BF[i][k])
                {
                    found = 1;
                    break;
                }
            }
            if (found == 0)
                count++;
        }
    }

    int number_of_edegs = N * K;
    double percent = (((double)number_of_edegs - (double)count) / (double)number_of_edegs) * 100;
    // cout << "\x1b[32msimilarity percentage: " << percent << "%"
    //      << "\x1b[0m" << endl;
    TEST_ASSERT((percent > 90.0));

    for (int i = 0; i < N; i++)
    {
        delete[] NND[i];
        delete[] BF[i];
    }

    delete[] NND;
    delete[] BF;

    end_program();
}

void set_max()
{
    Set set = set_create(compare_ints, delete_int);

    SetNode max_node = set_max(set);
    TEST_ASSERT(max_node == NULL);

    set_insert(set, create_int(20));
    max_node = set_max(set);
    TEST_ASSERT(*(int *)set_node_value(set, max_node) == 20);

    int n = 20;
    set_remove(set, &n);
    max_node = set_max(set);
    TEST_ASSERT(max_node == NULL);

    set_insert(set, create_int(100));
    max_node = set_max(set);
    TEST_ASSERT(*(int *)set_node_value(set, max_node) == 100);

    set_insert(set, create_int(60));
    max_node = set_max(set);
    TEST_ASSERT(*(int *)set_node_value(set, max_node) == 100);

    set_insert(set, create_int(140));
    max_node = set_max(set);
    TEST_ASSERT(*(int *)set_node_value(set, max_node) == 140);

    set_insert(set, create_int(50));
    max_node = set_max(set);
    TEST_ASSERT(*(int *)set_node_value(set, max_node) == 140);

    set_insert(set, create_int(90));
    max_node = set_max(set);
    TEST_ASSERT(*(int *)set_node_value(set, max_node) == 140);

    set_insert(set, create_int(150));
    max_node = set_max(set);
    TEST_ASSERT(*(int *)set_node_value(set, max_node) == 150);

    set_insert(set, create_int(40));
    max_node = set_max(set);
    TEST_ASSERT(*(int *)set_node_value(set, max_node) == 150);

    set_insert(set, create_int(120));
    max_node = set_max(set);
    TEST_ASSERT(*(int *)set_node_value(set, max_node) == 150);

    set_insert(set, create_int(110));
    max_node = set_max(set);
    TEST_ASSERT(*(int *)set_node_value(set, max_node) == 150);

    set_insert(set, create_int(80));
    max_node = set_max(set);
    TEST_ASSERT(*(int *)set_node_value(set, max_node) == 150);

    set_insert(set, create_int(160));
    max_node = set_max(set);
    TEST_ASSERT(*(int *)set_node_value(set, max_node) == 160);

    set_insert(set, create_int(70));
    max_node = set_max(set);
    TEST_ASSERT(*(int *)set_node_value(set, max_node) == 160);

    set_insert(set, create_int(130));
    max_node = set_max(set);
    TEST_ASSERT(*(int *)set_node_value(set, max_node) == 160);

    for (int i = 0; i < 12; i++)
    {
        n = ((13 - i) * 10) + 30;
        set_remove(set, &n);
        max_node = set_max(set);
        TEST_ASSERT(*(int *)set_node_value(set, max_node) == (n - 10));
    }

    set_remove(set, set_node_value(set, set_max(set)));
    TEST_ASSERT(set_max(set) == NULL);

    set_destroy(set);
}

void test_create(void)
{

    // Δημιουργούμε ένα κενό set (χωρίς συνάρτηση αποδεύσμευσης)
    Set set = set_create(compare_ints, NULL);
    set_set_destroy_value(set, NULL);

    TEST_ASSERT(set != NULL);
    TEST_ASSERT(set_size(set) == 0);

    set_destroy(set);
}

void test_insert(void)
{

    Set set = set_create(compare_ints, delete_int);

    int N = 1000;

    int **value_array = new int *[N];
    // Δοκιμάζουμε την insert με νέες τιμές κάθε φορά και με αυτόματο free
    for (int i = 0; i < N; i++)
    {

        value_array[i] = create_int(i);

        insert_and_test(set, value_array[i]);

        TEST_ASSERT(set_size(set) == (i + 1));
    }

    // Δοκιμάζουμε την insert με τιμές που υπάρχουν ήδη στο Set
    // και ελέγχουμε ότι δεν ενημερώθηκε το size (καθώς δεν προστέθηκε νέος κόμβος)
    int *new_value = create_int(0);
    insert_and_test(set, new_value);

    TEST_ASSERT(set_size(set) == N);

    set_destroy(set);

    // Δοκιμάζουμε την insert χωρίς αυτόματο free
    Set set2 = set_create(compare_ints, NULL);

    int local_value1 = 0, local_value2 = 1, local_value3 = 1;

    insert_and_test(set2, &local_value1);
    insert_and_test(set2, &local_value2);
    insert_and_test(set2, &local_value3); // ισοδύναμη τιμή => replace

    set_destroy(set2);
    delete_int(new_value);
    delete[] value_array;
}

void test_remove(void)
{

    Set set = set_create(compare_ints, delete_int);

    int N = 1000;

    int **value_array = new int *[N];

    for (int i = 0; i < N; i++)
        value_array[i] = create_int(i);

    // Ανακατεύουμε το value_array ώστε να υπάρχει ομοιόμορφη εισαγωγή τιμών
    // Πχ εάν εισάγουμε δείκτες με αύξουσα σειρά τιμών, τότε εάν το Set υλοποιείται με BST,
    // οι κόμβοι θα προστίθενται μόνο δεξιά της ρίζας, άρα και η set_remove δεν θα δοκιμάζεται πλήρως
    shuffle(value_array, N);

    for (int i = 0; i < N; i++)
        set_insert(set, value_array[i]);

    // Δοκιμάζουμε, πριν διαγράψουμε κανονικά τους κόμβους, ότι η set_remove
    // διαχειρίζεται σωστά μια τιμή που δεν υπάρχει στο Set
    int not_exists = 2000;
    TEST_ASSERT(!set_remove(set, &not_exists));

    // Διαγράφουμε όλους τους κόμβους
    for (int i = 0; i < N; i++)
    {
        TEST_ASSERT(set_remove(set, value_array[i]));
        TEST_ASSERT(set_is_proper(set));
    }

    set_destroy(set);

    // Δοκιμάζουμε τη remove χωρίς αυτόματο free
    Set set2 = set_create(compare_ints, NULL);

    int local_value1 = 0;

    insert_and_test(set2, &local_value1);
    TEST_ASSERT(set_remove(set2, &local_value1));
    TEST_ASSERT(set_is_proper(set2));
    TEST_ASSERT(set_size(set2) == 0);

    set_destroy(set2);
    delete[] value_array;
}

void test_find(void)
{

    Set set = set_create(compare_ints, delete_int);

    int N = 1000;

    int **value_array = new int *[N];

    for (int i = 0; i < N; i++)
        value_array[i] = create_int(i);

    // Παρόμοια με την set_remove, εάν το δέντρο δεν είναι σωστά ισορροπημένο, οι συναρτήσεις εύρεσης
    // στοιχείων δεν θα ελέγχονται πλήρως
    shuffle(value_array, N);

    for (int i = 0; i < N; i++)
    {
        set_insert(set, value_array[i]);

        SetNode found_node = set_find_node(set, value_array[i]);
        Pointer found_value = set_node_value(set, found_node);

        TEST_ASSERT(found_node != SET_EOF);
        TEST_ASSERT(found_value == value_array[i]);
    }

    // Αναζήτηση στοιχείου που δεν υπάρχει στο set
    int not_exists = 2000;
    TEST_ASSERT(set_find_node(set, &not_exists) == SET_EOF);
    TEST_ASSERT(set_find(set, &not_exists) == NULL);

    // Αναζήτηση μέγιστων/ελάχιστων στοιχείων
    // Συγκρίνουμε τις τιμές των δεικτών και όχι τους ίδιους τους δείκτες, καθώς
    // δεν γνωρίζουμε την θέση τους μετά απο το ανακάτεμα του πίνακα, αλλά γνωρίζουμε
    // ποιές τιμές υπάρχουν στο Set. Στη συγκεκριμένη περίπτωση, γνωρίζουμε ότι Set = {0, 1, ..., N-1}
    SetNode first_node = set_first(set);
    Pointer first_value = set_node_value(set, first_node);
    TEST_ASSERT((*(int *)first_value) == 0);

    SetNode next = set_next(set, first_node);
    Pointer next_value = set_node_value(set, next);
    TEST_ASSERT((*(int *)next_value) == 1);

    SetNode last_node = set_max(set);
    Pointer last_node_value = set_node_value(set, last_node);
    TEST_ASSERT((*(int *)last_node_value) == N - 1);

    SetNode prev = set_previous(set, last_node);
    Pointer prev_value = set_node_value(set, prev);
    TEST_ASSERT((*(int *)prev_value) == N - 2);

    // Ελέγχουμε και ότι βρίσκουμε σωστά τις τιμές από ενδιάμεσους κόμβους
    SetNode middle_node = set_find_node(set, value_array[N / 2]);
    SetNode middle_node_prev = set_previous(set, middle_node);

    Pointer middle_node_value = set_node_value(set, middle_node);
    Pointer middle_node_value_prev = set_node_value(set, middle_node_prev);

    TEST_ASSERT(*(int *)middle_node_value == *(int *)middle_node_value_prev + 1);

    set_destroy(set);
    delete[] value_array;
}

void test_iterate(void)
{
    Set set = set_create(compare_ints, delete_int);

    int N = 1000;
    int **value_array = new int *[N];

    for (int i = 0; i < N; i++)
        value_array[i] = create_int(i);

    // εισαγωγή τιμών σε τυχαία σειρά
    shuffle(value_array, N);

    for (int i = 0; i < N; i++)
        set_insert(set, value_array[i]);

    // iterate, τα στοιχεία πρέπει να τα βρούμε στη σειρά διάταξης
    int i = 0;
    for (SetNode node = set_first(set); node != SET_EOF; node = set_next(set, node))
    {
        TEST_ASSERT(*(int *)set_node_value(set, node) == i++);
    }

    // Κάποια removes
    i = N - 1;
    set_remove(set, &i);
    i = 40;
    set_remove(set, &i);

    // iterate, αντίστροφη σειρά, τα στοιχεία πρέπει να τα βρούμε στη σειρά διάταξης
    i = N - 2;
    for (SetNode node = set_max(set); node != SET_EOF; node = set_previous(set, node))
    {
        if (i == 40)
            i--; // το 40 το έχουμε αφαιρέσει

        TEST_ASSERT(*(int *)set_node_value(set, node) == i--);
    }

    set_destroy(set);
    delete[] value_array;
}

void test_node_value(void)
{
    // Η συνάρτηση αυτή ελέγχει ότι ένας κόμβος περιέχει πάντα την αρχική του τιμή,
    // χωρίς να επηρρεάζεται από άλλους κόμβους που προστίθενται ή διαγράφονται.

    Set set = set_create(compare_ints, delete_int);

    int N = 1000;
    int **value_array = new int *[N];

    for (int i = 0; i < N; i++)
        value_array[i] = create_int(i);

    shuffle(value_array, N);

    // Εισάγουμε έναν αριθμό και αποθηκεύουμε το node
    set_insert(set, value_array[0]);
    SetNode node = set_first(set);
    TEST_ASSERT(set_node_value(set, node) == value_array[0]);

    // Προσθήκη τιμών, και έλεγχος μετά από κάθε προσθήκη
    for (int i = 1; i < N; i++)
    {
        set_insert(set, value_array[i]);

        TEST_ASSERT(set_node_value(set, node) == value_array[0]);
    }

    // Διαγραφή τιμών, και έλεγχος μετά από κάθε διαγραφή
    for (int i = 1; i < N; i++)
    {
        set_remove(set, value_array[i]);

        TEST_ASSERT(set_node_value(set, node) == value_array[0]);
    }

    set_destroy(set);
    delete[] value_array;
}

void test_Euclidean()
{
    const float point1[] = {3.0, 4.0, 0.0};
    const float point2[] = {3.0, 0.0, 0.0};
    int numDimensions = 3;

    double euclideanDistance = calculateEuclideanDistance(point1, point2, numDimensions);
    TEST_ASSERT(euclideanDistance == 4);
}

void test_Manhattan()
{
    const float point1[] = {1.0, 2.0, 3.0};
    const float point2[] = {4.0, 5.0, 6.0};
    int numDimensions = 3;

    double manhattanDistance = calculateManhattanDistance(point1, point2, numDimensions);
    TEST_ASSERT(manhattanDistance == 9);
}

void test_compare_ints()
{
    int num1 = 5;
    int num2 = 8;
    double result = compare_ints(&num1, &num2);
    TEST_ASSERT(result == -3);
}

void test_create_int()
{
    int *num = create_int(42);
    TEST_ASSERT(*num == 42);
    delete num;
}

void test_compare_distances()
{
    Neighbor neighbor1(1, 10.5);
    Neighbor neighbor2(2, 8.2);

    int result = compare_distances(&neighbor1, &neighbor2);
    TEST_ASSERT(result == 2300000);
}

void test_furthest_closest()
{
    Set mySet = set_create(compare_distances, NULL);

    // Adding some neighbors to the set for testing
    Neighbor neighbor1(1, 10.5);
    Neighbor neighbor2(2, 8.2);

    // Add neighbors to the set
    set_insert(mySet, &neighbor1);
    set_insert(mySet, &neighbor2);

    // Test furthest_neighbor and closest_neighbor
    Neighbor *furthest = furthest_neighbor(mySet);
    Neighbor *closest = closest_neighbor(mySet);

    TEST_ASSERT(*furthest->getid() == 1);
    TEST_ASSERT(*closest->getid() == 2);

    set_destroy(mySet);
}

void test_compare_results()
{
    const int N = 50;
    const int K = 2;

    // Initialize 2 arrays with known values
    int **arrayBF = new int *[N];
    int **arrayNND = new int *[N];

    int m = 0;

    for (int i = 0; i < N; i++)
    {
        arrayBF[i] = new int[K];
        arrayNND[i] = new int[K];
        for (int j = 0; j < K; j++)
        {
            arrayBF[i][j] = m;
            arrayNND[i][j] = m;
            m++;
        }
    }

    // and then modify one element
    arrayNND[1][0] = -1;

    // now we compare the arrays and we see that the similarity percentage is not 100%
    double similarityPercentage = compare_results(arrayBF, arrayNND, N, K);
    TEST_ASSERT(similarityPercentage == 99);

    arrayBF = new int *[N];
    arrayNND = new int *[N];

    m = 0;

    for (int i = 0; i < N; i++)
    {
        arrayBF[i] = new int[K];
        arrayNND[i] = new int[K];
        for (int j = 0; j < K; j++)
        {
            arrayBF[i][j] = m;
            arrayNND[i][j] = m;
            m++;
        }
    }

    arrayNND[1][0] = -1;
    arrayNND[2][1] = -1;
    arrayNND[4][0] = -1;
    arrayNND[4][1] = -1;

    // now we compare the arrays and we see that the similarity percentage is not 100%
    similarityPercentage = compare_results(arrayBF, arrayNND, N, K);
    TEST_ASSERT(similarityPercentage == 96);
}

void test_contains()
{
    Neighbor *narray[20];
    for (int i = 0; i < 20; i++)
    {
        narray[i] = new Neighbor(i, 0);
    }

    for (int i = 0; i < 20; i++)
    {
        TEST_ASSERT(contains(narray, 20, i));
    }

    for (int i = 21; i < 30; i++)
    {
        TEST_ASSERT(!contains(narray, 20, i));
    }

    for (int i = 0; i < 20; i++)
    {
        delete narray[i];
    }
}

void test_distance_results()
{
    const int N = 10;
    const int num_dimensions = 20;

    float **data = new float *[N];
    for (int i = 0; i < N; ++i)
    {
        data[i] = new float[num_dimensions];
        for (int j = 0; j < num_dimensions; ++j)
        {
            data[i][j] = static_cast<float>(rand()) / RAND_MAX;
        }
    }

    float **distResults;
    distResults = new float *[N * N];
    for (int i = 0; i < N; ++i)
    {
        distResults[i] = new float[N];
    }

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            distResults[i][j] = calculateEuclideanDistance(data[i], data[j], num_dimensions);
        }
    }

    srand(time(nullptr));

    for (int i = 0; i < 20; i++)
    {
        int point1 = rand() % N;
        int point2 = rand() % N;

        // bool testResult = testRandomPoints(data, distResults, num_dimensions, point1, point2);
        float calculatedDistance = calculateEuclideanDistance(data[point1], data[point2], num_dimensions);
        float storedDistance = distResults[point1][point2];
        TEST_ASSERT(calculatedDistance == storedDistance);
    }

    for (int i = 0; i < N; ++i)
    {
        delete[] data[i];
        delete[] distResults[i];
    }
    delete[] data;
    delete[] distResults;
}

void test_dot_product()
{
    float *vector1 = new float[5];
    for (int i = 0; i < 5; i++)
    {
        vector1[i] = 0.1 + i * 0.1;
    }

    float *vector2 = new float[5];
    for (int i = 0; i < 5; i++)
    {
        vector2[i] = 0.4 + i * 0.1;
    }

    TEST_ASSERT(compare_doubles(dot_product(vector1, vector2, 5), (double)1) == 0);

    vector1[0] = 0.1;
    vector1[1] = 0.35;
    vector1[2] = 0.61;
    vector1[3] = 0.23;
    vector1[4] = 0.157;
    vector2[0] = 0.82;
    vector2[1] = 0.5;
    vector2[2] = 0.21;
    vector2[3] = 0.07;
    vector2[4] = 0.07;

    TEST_ASSERT(compare_doubles(dot_product(vector1, vector2, 5), (double)0.41219) == 0);

    delete[] vector1;
    delete[] vector2;
}

void test_random_hyperplane()
{
    float *random_hyperplane_vec = new float[100];
    define_random_hyperplane(random_hyperplane_vec, 100, -0.4, 0.4);

    for (int i = 0; i < 100; i++)
    {
        TEST_ASSERT(random_hyperplane_vec[i] >= -0.4 && random_hyperplane_vec[i] <= 0.4);
    }

    delete[] random_hyperplane_vec;
}

void test_random_split()
{
    ifstream ifs;
    const char *file_path1 = "datasets/00000200.bin";
    ifs.open(file_path1, ios::binary);
    if (!ifs.is_open())
    {
        cout << "Failed to open the file." << endl;
        TEST_ASSERT(0);
        return;
    }

    // Read the number of points (N) and store it in the global variable
    ifs.read((char *)&N, sizeof(int));
    cout << "# of points: " << N << endl;

    const int num_dimensions = 100;

    float **_data;

    // Create arrays for storing the data
    _data = new float *[N];
    for (int i = 0; i < N; i++)
    {
        _data[i] = new float[num_dimensions];
    }

    for (int i = 0; i < N; i++)
    {
        for (int d = 0; d < num_dimensions; d++)
        {
            float value;
            ifs.read((char *)(&value), sizeof(float));
            _data[i][d] = value;
        }
    }

    ifs.close();

    Vertex **vertexArray = new Vertex *[N];
    for (int i = 0; i < N; i++)
    {
        vertexArray[i] = new Vertex(_data[i], i);
    }

    TreeNode rp_root = new tree_node(num_dimensions, vertexArray, N, 25);
    rp_root->random_projection_split();

    TreeNode _left_sub = rp_root->left_sub();
    TreeNode _right_sub = rp_root->right_sub();

    TEST_ASSERT(_left_sub->numDataPoints > 0 && _left_sub->numDataPoints < rp_root->numDataPoints);
    TEST_ASSERT(_right_sub->numDataPoints > 0 && _right_sub->numDataPoints < rp_root->numDataPoints);

    TEST_ASSERT(_left_sub->left_sub() == NULL && _left_sub->right_sub() == NULL);
    TEST_ASSERT(_right_sub->left_sub() == NULL && _right_sub->right_sub() == NULL);

    for (int i = 0; i < N; ++i)
    {
        delete[] _data[i];
    }
    delete[] _data;

    rp_root->delete_tree();

    for (int i = 0; i < N; i++)
    {
        delete vertexArray[i];
    }
    delete[] vertexArray;
}

float calculate_average_distance(Vertex **data, int numDataPoints)
{
    float totalDist = 0.0;
    int pairs = 0;

    for (int i = 0; i < numDataPoints; i++)
    {
        for (int j = i + 1; j < numDataPoints; j++)
        {
            Vertex *v1 = data[i];
            Vertex *v2 = data[j];
            float *d1 = static_cast<float *>(v1->getData());
            float *d2 = static_cast<float *>(v2->getData());

            float distance = calculateEuclideanDistance(d1, d2, 3);
            totalDist += distance;
            pairs++;
        }
    }

    if (pairs > 0)
        return totalDist / pairs;
    return 0.0;
}

void test_tree_rec1()
{
    // srand(time(nullptr));
    int num = 200;
    float **_data = new float *[num];
    for (int i = 0; i < num; i++)
    {
        _data[i] = new float[3];
        for (int j = 0; j < 3; j++)
        {
            _data[i][j] = generate_random_float(-0.4, 0.4);
        }
    }

    Vertex **vertexArray = new Vertex *[num];
    for (int i = 0; i < num; i++)
    {
        vertexArray[i] = new Vertex(_data[i], i);
    }

    int *index = new int;
    *index = 0;
    TreeNode *leaf_array = new TreeNode[num];
    int leaf_size_limit = 25;

    TreeNode rp_root = new tree_node(3, vertexArray, num, leaf_size_limit);
    rp_root->rp_tree_rec(index, leaf_array);

    // cout << "index after leaf ineserts: " << *index << endl;

    // extra test to prove the correctness of the random projection tree.

    int count = 0;
    for (int i = 0; i < *index; i++)
    {
        TEST_ASSERT(leaf_array[i]->numDataPoints <= leaf_size_limit);
        Vertex **varr = leaf_array[i]->get_data();
        // cout << "leaf " << i << " size: " << leaf_array[i]->numDataPoints << endl;
        for (int j = 0; j < leaf_array[i]->numDataPoints; j++)
        {
            Vertex *v = varr[j];
            count += v->getId();
            // cout << v->getId() << ", ";
        }

        if (leaf_array[i]->numDataPoints > 1)
        {
            float averageDist = calculate_average_distance(varr, leaf_array[i]->numDataPoints);
            cout << "Average distance in leaf " << i << ": " << averageDist << endl;
            // TEST_ASSERT(averageDist < 0.5);
        }
    }

    int sum = num * (num - 1) / 2;
    TEST_ASSERT(sum == count);

    // -------------------------------------------------------------------

    for (int i = 0; i < num; ++i)
    {
        delete[] _data[i];
    }
    delete[] _data;

    for (int i = 0; i < num; i++)
    {
        delete vertexArray[i];
    }
    delete[] vertexArray;

    rp_root->delete_tree();

    delete index;
    delete[] leaf_array;
}

void test_tree_rec2()
{
    ifstream ifs;
    const char *file_path1 = "datasets/00000200.bin";
    ifs.open(file_path1, ios::binary);
    if (!ifs.is_open())
    {
        cout << "Failed to open the file." << endl;
        TEST_ASSERT(0);
        return;
    }

    // Read the number of points (N) and store it in the global variable
    ifs.read((char *)&N, sizeof(int));
    cout << "# of points: " << N << endl;

    const int num_dimensions = 100;

    float **_data;

    // Create arrays for storing the data
    _data = new float *[N];
    for (int i = 0; i < N; i++)
    {
        _data[i] = new float[num_dimensions];
    }

    for (int i = 0; i < N; i++)
    {
        for (int d = 0; d < num_dimensions; d++)
        {
            float value;
            ifs.read((char *)(&value), sizeof(float));
            _data[i][d] = value;
        }
    }

    ifs.close();

    Vertex **vertexArray = new Vertex *[N];
    for (int i = 0; i < N; i++)
    {
        vertexArray[i] = new Vertex(_data[i], i);
    }

    int *index = new int;
    *index = 0;
    TreeNode *leaf_array = new TreeNode[N];
    int leaf_size_limit = 25;

    TreeNode rp_root = new tree_node(100, vertexArray, N, leaf_size_limit);
    rp_root->rp_tree_rec(index, leaf_array);

    cout << "index after leaf ineserts: " << *index << endl;

    // extra test to prove the correctness of the random projection tree.
    int count = 0;
    for (int i = 0; i < *index; i++)
    {
        TEST_ASSERT(leaf_array[i]->numDataPoints <= leaf_size_limit);
        Vertex **varr = leaf_array[i]->get_data();
        // cout << "leaf " << i << " size: " << leaf_array[i]->numDataPoints << endl;
        for (int j = 0; j < leaf_array[i]->numDataPoints; j++)
        {
            Vertex *v = varr[j];
            count += v->getId();
            // cout << v->getId() << ", ";
        }

        if (leaf_array[i]->numDataPoints > 1)
        {
            float averageDist = calculate_average_distance(varr, leaf_array[i]->numDataPoints);
            cout << "Average distance in leaf " << i << ": " << averageDist << endl;
            // TEST_ASSERT(averageDist < 0.5);
        }
    }

    //----------------------------------------------------------------------------------------

    for (int i = 0; i < N; ++i)
    {
        delete[] _data[i];
    }
    delete[] _data;

    for (int i = 0; i < N; i++)
    {
        delete vertexArray[i];
    }
    delete[] vertexArray;

    rp_root->delete_tree();

    delete index;
    delete[] leaf_array;
}

void test_RPGraph()
{
    DistanceFunction distanceFunction = &calculateEuclideanDistance;

    ifstream ifs;
    const char *file_path1 = "datasets/00000200.bin";
    ifs.open(file_path1, ios::binary);
    if (!ifs.is_open())
    {
        cout << "Failed to open the file." << endl;
        TEST_ASSERT(0);
        return;
    }

    // Read the number of points (N) and store it in the global variable
    ifs.read((char *)&N, sizeof(int));
    cout << "# of points: " << N << endl;

    const int num_dimensions = 100;

    float **_data;

    // Create arrays for storing the data
    _data = new float *[N];
    for (int i = 0; i < N; i++)
    {
        _data[i] = new float[num_dimensions];
    }

    for (int i = 0; i < N; i++)
    {
        for (int d = 0; d < num_dimensions; d++)
        {
            float value;
            ifs.read((char *)(&value), sizeof(float));
            _data[i][d] = value;
        }
    }

    ifs.close();

    int rp_limit = 25;
    int K = 40;

    KNNDescent KNNGraph(K, N, 0.4, num_dimensions, _data, distanceFunction, 0.001, rp_limit);

    for (int i = 0; i < N; i++)
    {
        KNNGraph.vertexArray[i] = new Vertex(_data[i], i);
    }

    TreeNode rp_root = new tree_node(num_dimensions, KNNGraph.vertexArray, N, rp_limit);

    int expected_leaves = 0.5 * N;
    TreeNode *leaf_array = new TreeNode[expected_leaves];

    int *index = new int;
    *index = 0;

    rp_root->rp_tree_rec(index, leaf_array);

    // cout << "index after leaf ineserts: " << *index << endl;

    int total_count = 0;
    for (int i = 0; i < *index; i++)
    {
        total_count += leaf_array[i]->numDataPoints;
    }

    TEST_ASSERT(total_count == N);

    // for each leaf of the tree
    for (int i = 0; i < *index; i++)
    {
        Vertex **leaf_data = leaf_array[i]->get_data();
        int data_count = leaf_array[i]->numDataPoints;

        // for each datapoint of the cluster assign all the other datapoints of the cluster as its neighbors
        for (int j = 0; j < data_count; j++)
        {
            Vertex *v1 = leaf_data[j];
            float *d1 = static_cast<float *>(v1->getData());
            int v1_id = v1->getId();

            for (int k = 0; k < data_count; k++)
            {
                if (j == k)
                    continue;

                Vertex *v2 = leaf_data[k];
                float *d2 = static_cast<float *>(v2->getData());
                int v2_id = v2->getId();

                float dist = distanceFunction(d1, d2, num_dimensions);

                Neighbor *newNeighbor = new Neighbor(v2_id, dist);
                v1->addNeighbor(newNeighbor);

                Neighbor *newReverseNeighbor = new Neighbor(v1_id, dist);
                v2->addReverseNeighbor(newReverseNeighbor);
            }

            Set nn = v1->getNeighbors();
            TEST_ASSERT(set_size(nn) == data_count - 1);

            // fill in the remaining neighbors with random ones from the graph
            while (set_size(nn) < K)
            {
                // pick a random leaf
                int random_idx = random_int(*index, i);
                TEST_ASSERT(random_idx != i && random_idx >= 0 && random_idx < *index);

                Vertex **random_leaf_data = leaf_array[random_idx]->get_data();

                // pick a random datapoint
                int data_idx = rand() % leaf_array[random_idx]->numDataPoints;
                Vertex *rv = random_leaf_data[data_idx];

                float *rd = static_cast<float *>(rv->getData());
                int rv_id = rv->getId();

                float dist = distanceFunction(d1, rd, num_dimensions);

                Neighbor *newNeighbor = new Neighbor(rv_id, dist);
                if (!v1->addNeighbor(newNeighbor))
                {
                    continue;
                }

                Neighbor *newReverseNeighbor = new Neighbor(v1_id, dist);
                rv->addReverseNeighbor(newReverseNeighbor);
            }

            TEST_ASSERT(set_size(nn) == K);
        }

        for (int j = 0; j < data_count; j++)
        {
            Vertex *v = leaf_data[j];
            Set rn = v->getReverseNeighbors();
            TEST_ASSERT(set_size(rn) >= data_count - 1);
        }
    }

    //----------------------------------------------------
    for (int i = 0; i < N; i++)
    {
        delete[] _data[i];
    }
    delete[] _data;

    rp_root->delete_tree();

    delete index;
    delete[] leaf_array;
}

void test_random_int()
{
    for (int i = 0; i < 20; i++)
    {
        int random = random_int(20, i);
        TEST_ASSERT(random != i && random >= 0 && random < 20);
    }
}

void test_rp_similarity()
{
    srand(time(nullptr));

    ifstream ifs;
    const char *file_path1 = "datasets/00001000-1.bin";
    ifs.open(file_path1, ios::binary);
    if (!ifs.is_open())
    {
        cout << "Failed to open the file." << endl;
        TEST_ASSERT(0);
        return;
    }

    // Read the number of points (N) and store it in the global variable
    ifs.read((char *)&N, sizeof(int));
    cout << "# of points: " << N << endl;

    const int num_dimensions = 100;

    float **_data;

    // Create arrays for storing the data
    _data = new float *[N];
    for (int i = 0; i < N; i++)
    {
        _data[i] = new float[num_dimensions];
    }

    for (int i = 0; i < N; i++)
    {
        for (int d = 0; d < num_dimensions; d++)
        {
            float value;
            ifs.read((char *)(&value), sizeof(float));
            _data[i][d] = value;
        }
    }

    ifs.close();
    DistanceFunction distanceFunction = &calculateEuclideanDistance;
    K = 100;
    p = 0.5;

    KNNDescent KNNGraph1(100, N, p, num_dimensions, _data, distanceFunction, 0.001, 90);
    KNNDescent KNNGraph2(100, N, p, num_dimensions, _data, distanceFunction, 0.001, 90);
    KNNBruteForce BFGraph(100, N, num_dimensions, _data, distanceFunction);

    // compare similarities between random projection tree init and random init
    KNNGraph1.createRandomGraph();
    KNNGraph2.createRPGraph();

    // cout << "KNNGraph1 (random init)" << endl;
    // for (int i = 0; i < N; i++)
    // {
    //     Vertex *v = KNNGraph1.vertexArray[i];
    //     int vid = v->getId();
    //     Set nn = v->getNeighbors();
    //     cout << "average neighbor distance of vertex " << vid << ": " << averageNeighborDistance(nn) << endl;
    // }
    // cout << endl;

    // cout << "KNNGraph2 (rp tree init)" << endl;
    // for (int i = 0; i < N; i++)
    // {
    //     Vertex *v = KNNGraph2.vertexArray[i];
    //     int vid = v->getId();
    //     Set nn = v->getNeighbors();
    //     cout << "average neighbor distance of vertex " << vid << ": " << averageNeighborDistance(nn) << endl;
    // }
    // cout << endl;

    cout << "BRUTE FORCE GRAPH" << endl;
    for (int i = 0; i < N; i++)
    {
        Vertex *v = BFGraph.vertexArray[i];
        int vid = v->getId();
        Set nn = v->getNeighbors();
        // cout << "average neighbor distance of vertex " << vid << ": " << averageNeighborDistance(nn) << endl;
    }
    // cout << endl;

    int **NND1 = KNNGraph1.extract_neighbors_to_list();
    int **NND2 = KNNGraph2.extract_neighbors_to_list();

    int **BF1 = BFGraph.extract_neighbors_to_list();
    int **BF2 = BFGraph.extract_neighbors_to_list();

    double percentage1 = compare_results(BF1, NND1, (int)N, K);
    double percentage2 = compare_results(BF2, NND2, (int)N, K);

    if (percentage1 > 90.0)
    {
        cout << "\x1b[32mrandom graph init similarity percentage: " << percentage1 << "%"
             << "\x1b[0m" << endl;
    }
    else
    {
        cout << "\x1b[31mrandom graph init similarity percentage: " << percentage1 << "%"
             << "\x1b[0m" << endl;
    }

    if (percentage2 > 90.0)
    {
        cout << "\x1b[32mrandom projection tree init similarity percentage: " << percentage2 << "%"
             << "\x1b[0m" << endl;
    }
    else
    {
        cout << "\x1b[31mrandom projection tree init similarity percentage: " << percentage2 << "%"
             << "\x1b[0m" << endl;
    }

    //---------------------------------------------------------------------------------------------------------
    for (int i = 0; i < N; i++)
    {
        delete[] _data[i];
    }
    delete[] _data;
}

TEST_LIST = {
    // {"test distances", test_distances},
    // {"test_potential", test_potential},
    // {"test_contains", test_contains},
    // {"test_result", test_result},
    // {"test_euclidean", test_Euclidean},
    // {"test_distance_results", test_distance_results},
    // {"test_manhattan", test_Manhattan},
    // {"test_compare_ints", test_compare_ints},
    // {"test_create_int", test_create_int},
    // {"test_compare_distances", test_compare_distances},
    // {"test_furthest_closest", test_furthest_closest},
    // {"test_compare_results", test_compare_results},
    // {"set_max", set_max},
    // {"set_create", test_create},
    // {"set_insert", test_insert},
    // {"set_remove", test_remove},
    // {"set_find", test_find},
    // {"set_iterate", test_iterate},
    // {"set_node_value", test_node_value},

    // {"test_dot_product", test_dot_product},
    // {"test_random_hyperplane", test_random_hyperplane},
    // {"test_random_split", test_random_split},
    // {"test_tree_rec1", test_tree_rec1},
    // {"test_tree_rec2", test_tree_rec2},
    // {"test_RPGraph", test_RPGraph},
    // {"test_random_int", test_random_int},
    {"test_rp_similarity", test_rp_similarity},
    {NULL, NULL}};