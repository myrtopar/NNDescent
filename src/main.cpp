#include <iostream>
#include "headers/KNNGraph.hpp"

using namespace std;
float **distanceResults;

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

float calculateManhattanDistance(const float *point1, const float *point2, int numDimensions)
{
    double sum = 0.0;
    for (int i = 0; i < numDimensions; i++)
    {
        sum += fabs(point1[i] - point2[i]);
    }
    return sum;
}

void calculateALLdistances(float **data, int N, int num_dimensions)
{
    distanceResults = new float *[N * N];
    for (int i = 0; i < N; ++i)
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


int main(int argc, char *argv[])
{

    if (argc != 7)
    {
        cout << "Error wrong amount of arguments.\n";
        return -1;
    }

    int K = atoi(argv[1]);
    float p = stof(argv[2]);
    int metric = atoi(argv[3]); // to encode
    char *file_path = argv[4];
    double delta = stof(argv[5]);
    int rp_limit = atoi(argv[6]);

    if (p > 1.0)
    {
        cout << "Error. Wrong sampling percent." << endl;
        return EXIT_FAILURE;
    }

    ifstream ifs;
    ifs.open(file_path, ios::binary);
    if (!ifs.is_open())
    {
        cout << "Failed to open the file." << endl;
        return 1;
    }

    // Read the number of points (N)
    uint32_t N;
    ifs.read((char *)&N, sizeof(uint32_t));

    const int num_dimensions = 100;

    // Create arrays for storing the data
    float **data = new float *[N];
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

    if (K > (int)N)
    {
        cout << "Error, invalid K." << endl;
        delete_data(data, N);
        return -1;
    }

    DistanceFunction distanceFunction;
    if (metric == 1)
        distanceFunction = &calculateEuclideanDistance;
    else if (metric == 2)
        distanceFunction = &calculateManhattanDistance;
    else
    {
        cout << "Error. Calculate Distance Function doesn't exist.";
        return -1;
    }

    calculateALLdistances(data, N, num_dimensions);

    // knn descent method
    auto start1 = std::chrono::high_resolution_clock::now();
    KNNDescent KNNGraph(K, N, p, num_dimensions, data, distanceFunction, delta, rp_limit);
    KNNGraph.createKNNGraph();
    auto stop1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::seconds>(stop1 - start1);
    cout << "KNNDescent: " << duration1.count() << " seconds" << endl;

    // extract knn descent and brute force method results into a list and compare them, to find similarity percentage
    int **NND = KNNGraph.extract_neighbors_to_list();

    int **BF; // we might have calculated the result of this dataset with the Brute Force method

    if ((int)N <= 5000)
    {
        string pathname = argv[4];
        string _K = argv[1];
        string numbers = pathname.substr(pathname.find_last_of('/') + 1, 10);

        // Construct the output binary file path
        string resfile_path = "BF_results/" + numbers + "_" + _K + ".bin";

        ifstream file_check(resfile_path, ios::binary);
        if (file_check.good())
        {
            cout << "\nFile exists: " << resfile_path << endl;

            // read the content of the binary file with the results
            int size = N;
            BF = new int *[size];
            for (int i = 0; i < size; i++)
            {
                BF[i] = new int[K];
            }

            int value;
            int count = 0;
            int i = 0;
            while (file_check.read(reinterpret_cast<char *>(&value), sizeof(int)))
            {
                if (i == (int)N)
                { // at the end of each
                    cout << "Brute Force: " << value << " seconds" << endl;
                    break;
                }
                BF[i][count++] = value;
                if (count == K)
                {
                    count = 0;
                    i++;
                }
            }

            file_check.close();
        }
        else
        {
            cout << "\nFile does not exist: " << resfile_path << endl;

            auto start2 = std::chrono::high_resolution_clock::now();
            KNNBruteForce myGraph(K, N, num_dimensions, data, distanceFunction);
            auto stop2 = std::chrono::high_resolution_clock::now();
            auto duration2 = std::chrono::duration_cast<std::chrono::seconds>(stop2 - start2);

            cout << "Brute Force: " << duration2.count() << " seconds" << endl;

            ofstream ofs(resfile_path, ios::binary);

            if (!ofs.is_open())
            {
                cout << "Error opening file: " << resfile_path << endl;
                return EXIT_FAILURE;
            }

            BF = myGraph.extract_neighbors_to_list();

            for (int i = 0; i < (int)N; i++)
            {
                for (int k = 0; k < K; k++)
                {
                    int data_to_write = BF[i][k];
                    ofs.write(reinterpret_cast<const char *>(&data_to_write), sizeof(int));
                }
            }

            auto duration_value = duration2.count();
            ofs.write(reinterpret_cast<const char *>(&duration_value), sizeof(double));

            ofs.close();

        }

        double percentage = compare_results(BF, NND, (int)N, K);
        if (percentage > 90.0)
        {
            cout << "\x1b[32msimilarity percentage: " << percentage << "%" << "\x1b[0m" << endl;
        }
        else
        {
            cout << "\x1b[31msimilarity percentage: " << percentage << "%" << "\x1b[0m" << endl;
        }

    }

    // void **narray = KNNGraph.NNSinglePoint(data[10]);

    delete_data(data, N);

    return 0;
}