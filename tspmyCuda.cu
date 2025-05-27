#include <iostream>
#include <vector>
#include <fstream>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <curand_kernel.h>

#define NMAX 1000

__device__ int calculateTimeToTravel(int* matrix, int from, int to, int N) {
    if (matrix[from * N + to] == 0 || from == to)
        return -1;
    return matrix[from * N + to];
}

__global__ void tspKernel(int *matrix, int N, int *results, int *paths, int ITERATIONS) {  
    int tid = threadIdx.x + blockIdx.x * blockDim.x;
    if (tid >= ITERATIONS) return;

    bool visited[NMAX];
    int path[NMAX + 1];
    int iterators[NMAX];
    int total_time = 0;
    int curr = 0;

    for (int i = 0; i < N; i++) {
        visited[i] = false;
        iterators[i] = i;
    }

    visited[0] = true;
    path[0] = 0;

    curandState state;
    curand_init(clock64(), tid, 0, &state);

    for (int step = 1; step < N; step++) {
        for (int i = N - 1; i > 0; i--) {
            int j = curand(&state) % (i + 1);
            int tmp = iterators[i];
            iterators[i] = iterators[j];
            iterators[j] = tmp;
        }

        int min_time = INT_MAX;
        int next_vertex = -1;

        for (int i = 0; i < N; ++i) {
            int neighbour = iterators[i];
            int time = calculateTimeToTravel(matrix, curr, neighbour, N);
            if (time != -1 && !visited[neighbour] && time < min_time) {
                min_time = time;
                next_vertex = neighbour;
            }
        }

        if (next_vertex == -1) {
            results[tid] = INT_MAX;
            return;
        }

        total_time += min_time;
        curr = next_vertex;
        visited[curr] = true;
        path[step] = curr;
    }

    int return_time = calculateTimeToTravel(matrix, curr, 0, N);
    if (return_time == -1) {
        results[tid] = INT_MAX;
        return;
    }

    total_time += return_time;
    path[N] = 0;
    results[tid] = total_time;

    for (int i = 0; i <= N; i++)
        paths[tid * (N + 1) + i] = path[i];
}

void readMatrix(const std::string &filename, std::vector<int> &matrix, int &N) {
    std::ifstream file(filename);
    file >> N;
    matrix.resize(N * N);
    for (int i = 0; i < N * N; ++i)
        file >> matrix[i];
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <matrix_file>" << std::endl;
        return 1;
    }
    int ITERATIONS = atoi(argv[1]); // Liczba iteracji do wykonania
    int N;
    std::vector<int> matrix;
    readMatrix(argv[2], matrix, N);

    int *d_matrix, *d_results, *d_paths;
    cudaMalloc(&d_matrix, N * N * sizeof(int));
    cudaMalloc(&d_results, ITERATIONS * sizeof(int));
    cudaMalloc(&d_paths, ITERATIONS * (N + 1) * sizeof(int));
    cudaMemcpy(d_matrix, matrix.data(), N * N * sizeof(int), cudaMemcpyHostToDevice);

    int maxThreadsPerBlock;
    cudaDeviceGetAttribute(&maxThreadsPerBlock, cudaDevAttrMaxThreadsPerBlock, 0);
    std::cout << "Maksymalna liczba wątków w bloku (dla tego GPU): " << maxThreadsPerBlock << std::endl;

    int blockSize = maxThreadsPerBlock;
    int gridSize = (ITERATIONS + blockSize - 1) / blockSize;

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start);

    // Uruchomienie kernela z maksymalnym blockSize
    tspKernel<<<gridSize, blockSize>>>(d_matrix, N, d_results, d_paths, ITERATIONS);
    cudaDeviceSynchronize();

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);

    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);

    std::vector<int> results(ITERATIONS), paths(ITERATIONS * (N + 1));
    cudaMemcpy(results.data(), d_results, ITERATIONS * sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(paths.data(), d_paths, ITERATIONS * (N + 1) * sizeof(int), cudaMemcpyDeviceToHost);

    int min_time = INT_MAX, best_idx = -1;
    for (int i = 0; i < ITERATIONS; ++i) {
        if (results[i] < min_time) {
            min_time = results[i];
            best_idx = i;
        }
    }

    std::cout << "Najlepsza ścieżka: ";
    for (int i = 0; i <= N; ++i)
        std::cout << paths[best_idx * (N + 1) + i] << " ";
    std::cout << "\nCzas ścieżki: " << min_time << std::endl;

    std::cout << "Czas działania algorytmu (GPU): " << milliseconds / 1000 << " s" << std::endl;

    cudaFree(d_matrix);
    cudaFree(d_results);
    cudaFree(d_paths);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return 0;
}
