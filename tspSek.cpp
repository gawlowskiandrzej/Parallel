#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <tuple>
#include <cstdlib>
#include <climits>
#include <algorithm>
#include <sys/stat.h>

using namespace std;

class TSPSolver {
private:
    vector<vector<int> > adjmatrix;
    int N;
    int P;

    int calculateTimeToTravel(int i, int j) {
        return adjmatrix[i][j];
    }

public:
    TSPSolver(const vector<vector<int> >& matrix, int P_ = 1000) {
        adjmatrix = matrix;
        N = matrix.size();
        P = P_;
    }

    tuple<vector<int>, int, double> TSPGreedyRandomise() {
        vector<int> best_overall_path;
        int best_overall_time = INT_MAX;

        clock_t start_total = clock();

        for (int p = 0; p < P; ++p) {
            vector<bool> visited(N, false);
            vector<int> current_path;
            int start = 0;
            current_path.push_back(start);
            int curr = start;
            int total_time = 0;
            visited[start] = true;

            vector<int> iterators(N);
            for (int i = 0; i < N; ++i) iterators[i] = i;

            for (int step = 0; step < N - 1; ++step) {
                int min_time = INT_MAX;
                int next_vertex = -1;
                random_shuffle(iterators.begin(), iterators.end());

                for (int i = 0; i < N; ++i) {
                    int neighbour = iterators[i];
                    int time_to_travel = calculateTimeToTravel(curr, neighbour);
                    if (time_to_travel > -1 && !visited[neighbour] && time_to_travel < min_time) {
                        min_time = time_to_travel;
                        next_vertex = neighbour;
                    }
                }

                if (next_vertex == -1) break;

                curr = next_vertex;
                current_path.push_back(curr);
                visited[curr] = true;
                total_time += min_time;
            }

            if (current_path.size() == N) {
                int time_back = calculateTimeToTravel(curr, start);
                if (time_back > -1) {
                    current_path.push_back(start);
                    total_time += time_back;

                    if (total_time < best_overall_time) {
                        best_overall_path = current_path;
                        best_overall_time = total_time;
                    }
                }
            }
        }

        clock_t end_total = clock();
        double duration = double(end_total - start_total) / CLOCKS_PER_SEC;

        if (!best_overall_path.empty()) {
            return make_tuple(best_overall_path, best_overall_time, duration);
        } else {
            return make_tuple(vector<int>(), 0, 0.0);
        }
    }
};

bool file_exists(const string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

vector<vector<int> > load_matrix_from_file(const string& filename) {
    ifstream file(filename.c_str());
    if (!file.is_open()) throw runtime_error("Nie można otworzyć pliku.");

    string line;
    getline(file, line);
    int N = atoi(line.c_str());

    vector<vector<int> > matrix(N, vector<int>(N));
    for (int i = 0; i < N; ++i) {
        getline(file, line);
        stringstream ss(line);
        for (int j = 0; j < N; ++j) {
            ss >> matrix[i][j];
        }
    }

    file.close();
    return matrix;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Użycie: " << argv[0] << " plik1.txt plik2.txt ..." << endl;
        return 1;
    }

    srand(time(0));
    cout << "Wyniki dla poszczególnych plików:\n" << endl;

    for (int i = 2; i < argc; ++i) {
        string filename = argv[i];
        if (!file_exists(filename)) continue;

        try {
            vector<vector<int> > matrix = load_matrix_from_file(filename);
            TSPSolver solver(matrix, argv[1] ? atoi(argv[1]) : 1000);
            vector<int> path;
            int cost;
            double exec_time;
            tie(path, cost, exec_time) = solver.TSPGreedyRandomise();

            cout << "Plik: " << filename << endl;
            if (!path.empty()) {
                cout << "  Ścieżka        : ";
                for (size_t i = 0; i < path.size(); ++i)
                    cout << path[i] << (i + 1 < path.size() ? " -> " : "");
                cout << endl;

                cout << "  Całkowity koszt: " << cost << endl;
                cout << "  Czas wykonania : " << exec_time << "s\n" << endl;
            } else {
                cout << "  Nie znaleziono możliwej trasy.\n" << endl;
            }
        } catch (const exception& e) {
            cerr << "[Błąd] Nie można przetworzyć pliku " << filename << ": " << e.what() << endl;
        }
    }

    return 0;
}
