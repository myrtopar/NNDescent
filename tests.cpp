#include "acutest.h"
#include "classes.hpp"
#include "KNNGraph.hpp"

const char *file_path = "datasets/00000200.bin";
uint32_t N;
const int num_dimensions = 100;
float **data;
int K;

bool set_is_proper(Set set);

typedef double (*DistanceFunction)(const float *, const float *, int);
double calculateEuclideanDistance(const float *point1, const float *point2, int numDimensions)
{

    double sum = 0.0;
    for (int i = 0; i < numDimensions; i++)
    {
        double diff = point1[i] - point2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

// Έλεγχος της insert σε λιγότερο χώρο
void insert_and_test(Set set, Pointer value)
{

    set_insert(set, value);
    TEST_ASSERT(set_is_proper(set));
    TEST_ASSERT(set_find(set, value) == value);
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
    ifs.read((char *)&N, sizeof(uint32_t));
    cout << "# of points: " << N << endl;

    const int num_dimensions = 100;

    // Create arrays for storing the data
    data = new float *[N];
    for (uint32_t i = 0; i < N; i++)
    {
        data[i] = new float[num_dimensions];
    }

    for (uint32_t i = 0; i < N; i++)
    {
        for (int d = 0; d < num_dimensions; d++)
        {
            float value;
            ifs.read((char *)(&value), sizeof(float));
            data[i][d] = value;
        }
    }

    ifs.close();
}

void end_program(void)
{
    // Clean up data arrays
    for (uint32_t i = 0; i < N; i++)
    {
        delete[] data[i];
    }
    delete[] data;
}

// Tests if the neighbor sets hold the correct distaces
void test_distances(void)
{
    start_program();
    DistanceFunction distanceFunction = &calculateEuclideanDistance;

    KNNDescent<float, DistanceFunction> KNNGraph(10, N, num_dimensions, data, distanceFunction);

    Vertex **array = KNNGraph.vertexArray;

    for (int r = 0; r < 10; r++)
    {
        KNNGraph.calculatePotentialNewNeighbors();

        // array to store distances for each node
        double **nodeDistancesNN = new double *[N];
        double **nodeDistancesRN = new double *[N];
        double **nodeDistancesPN = new double *[N];

        for (uint32_t i = 0; i < N; i++)
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
                double *dist = n->getDistance();
                double exactDistance = calculateEuclideanDistance(data[i], data[*id], num_dimensions);
                nodeDistancesNN[i][j] = *dist;
                TEST_ASSERT((exactDistance - *dist) == 0);
                j++;
            }

            j = 0;
            for (SetNode node = set_first(rn); node != SET_EOF; node = set_next(rn, node))
            {
                Neighbor *n = (Neighbor *)set_node_value(rn, node);
                int *id = n->getid();
                double *dist = n->getDistance();
                double exactDistance = calculateEuclideanDistance(data[i], data[*id], num_dimensions);
                nodeDistancesRN[i][j] = *dist;
                TEST_ASSERT((exactDistance - *dist) == 0);
                j++;
            }

            j = 0;
            for (SetNode node = set_first(pn); node != SET_EOF; node = set_next(pn, node))
            {
                Neighbor *n = (Neighbor *)set_node_value(pn, node);
                int *id = n->getid();
                double *dist = n->getDistance();
                double exactDistance = calculateEuclideanDistance(data[i], data[*id], num_dimensions);
                nodeDistancesPN[i][j] = *dist;
                TEST_ASSERT((exactDistance - *dist) == 0);
                j++;
            }
        }

        if (KNNGraph.updateGraph() == 0)
            break;
    }

    end_program();
}

// Test if the potential neighbors set has been cleaned up, after updateGraph has been called
void test_potential()
{

    start_program();

    DistanceFunction distanceFunction = &calculateEuclideanDistance;
    KNNDescent<float, DistanceFunction> KNNGraph(10, N, num_dimensions, data, distanceFunction);

    for (int i = 0; i < 10; i++)
    {
        KNNGraph.calculatePotentialNewNeighbors();
        if (KNNGraph.updateGraph() == 0)
            break;

        for (uint32_t j = 0; j < N; j++)
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

    int K = 50;
    DistanceFunction distanceFunction = &calculateEuclideanDistance;

    // knn descent method
    auto start1 = std::chrono::high_resolution_clock::now();
    KNNDescent<float, DistanceFunction> KNNGraph(K, N, num_dimensions, data, distanceFunction);
    KNNGraph.createKNNGraph();
    auto stop1 = std::chrono::high_resolution_clock::now();

    auto duration1 = std::chrono::duration_cast<std::chrono::seconds>(stop1 - start1);
    cout << "KNNDescent: " << duration1.count() << " seconds" << endl;

    // brute force method
    auto start2 = std::chrono::high_resolution_clock::now();
    KNNBruteForce<float, DistanceFunction> myGraph(K, N, num_dimensions, data, distanceFunction);
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
    cout << "\x1b[32msimilarity percentage: " << percent << "%"
         << "\x1b[0m" << endl;
    TEST_ASSERT((percent > 90.0));

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

TEST_LIST = {
    {"test distances", test_distances},
    {"test_potential", test_potential},
    {"test_result", test_result},
    // {"set_max", set_max},
    // {"set_create", test_create},
    // {"set_insert", test_insert},
    // {"set_remove", test_remove},
    // {"set_find", test_find},
    // {"set_iterate", test_iterate},
    // {"set_node_value", test_node_value},
    {NULL, NULL}};