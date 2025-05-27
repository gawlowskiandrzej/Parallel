# Parallel

# How to compile
## SEK
g++ tspSek.cpp -o tsp
## OpenMP
g++ -o openmp -fopenmp ./tsp-openmp2.cpp
## CUDA
nvcc -O2 -o tspCuda ./tspmyCuda.cu

# How to run
## SEK
./tsp 1000 ./dane100.txt
## OpenMP
./openmp 1000 ./dane100.txt
## CUDA
./tspCuda 1000 ./dane1000.txt

## parameters
* number of iterations for every instance of problem
* input file name
