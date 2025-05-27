# Parallel

#### How to compile
* SEK <br>
g++ tspSek.cpp -o tsp
* OpenMP <br>
g++ -o openmp -fopenmp ./tsp-openmp2.cpp
* CUDA <br>
nvcc -O2 -o tspCuda ./tspmyCuda.cu

#### How to run
* SEK <br>
./tsp 1000 ./dane100.txt
* OpenMP <br>
./openmp 1000 ./dane100.txt
* CUDA <br>
./tspCuda 1000 ./dane1000.txt

#### Data generator
* python dataGenerate.py 1000 <br>
parameter is number of verticies

#### parameters
* number of iterations for every instance of problem
* input file name
