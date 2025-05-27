#include <iostream>
#include <vector>
#include <fstream>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <ctime>
#include <omp.h>

using namespace std;

int calculateTimeToTravel(const vector<vector<int>> &adjmatrix, int from, int to) {
    if (adjmatrix[from][to] == 0 || from == to)
        return -1;
    return adjmatrix[from][to];
}

vector<int> TSPGreedyRandomise(const vector<vector<int>> &adjmatrix, int N, int num_iterations, int& timet) {
    vector<int> best_path_overall;
    int min_total_time = INT_MAX;

    #pragma omp parallel
    {
        unsigned int seed = time(NULL) ^ omp_get_thread_num();

        #pragma omp for
        for (int it = 0; it < num_iterations; ++it) {
            vector<bool> visited(N, false);
            vector<int> path;
            int start = 0;
            int curr = start;
            int total_time = 0;
            visited[start] = true;
            path.push_back(start);

            vector<int> iterators(N);
            for (int i = 0; i < N; ++i) iterators[i] = i;

            for (int step = 1; step < N; ++step) {
                int min_time = INT_MAX;
                int next_vertex = -1;

                random_shuffle(iterators.begin(), iterators.end(), [&](int i){ return rand_r(&seed) % i; });

                for (int j = 0; j < N; ++j) {
                    int neighbour = iterators[j];
                    int time = calculateTimeToTravel(adjmatrix, curr, neighbour);
                    if (time != -1 && !visited[neighbour] && time < min_time) {
                        min_time = time;
                        next_vertex = neighbour;
                    }
                }

                if (next_vertex == -1) break;

                visited[next_vertex] = true;
                path.push_back(next_vertex);
                total_time += min_time;
                curr = next_vertex;
            }

            int return_time = calculateTimeToTravel(adjmatrix, curr, start);
            if (return_time != -1) {
                total_time += return_time;
                path.push_back(start);

                #pragma omp critical
                {
                    if (total_time < min_total_time) {
                        min_total_time = total_time;
                        best_path_overall = path;
                    }
                }
            }
        }
    }
    timet = min_total_time;
    return best_path_overall;
}

vector<vector<int>> readMatrix(const string &filename, int &N) {
    ifstream file(filename);
    file >> N;
    vector<vector<int>> matrix(N, vector<int>(N));
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            file >> matrix[i][j];
    return matrix;
}

int main(int argc, char *argv[]) {
    int N;
    vector<vector<int>> adjmatrix = readMatrix(argv[2], N);
    double start = omp_get_wtime();
    int time = 0;
    int iterations = atoi(argv[1]);
    vector<int> path = TSPGreedyRandomise(adjmatrix, N, iterations, time);

    cout << "Najlepsza ścieżka: ";
    for (int city : path) cout << city << " ";
    cout << "\nCzas wykonania: " << (omp_get_wtime() - start) << " sekund\n";

    return 0;
}