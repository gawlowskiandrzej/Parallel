# -*- coding: utf-8 -*-
import random
import sys

def generate_graph(n, min_weight=1, max_weight=100):
    graph = [[0 if i == j else None for j in range(n)] for i in range(n)]

    for i in range(n):
        for j in range(i + 1, n):
            weight = random.randint(min_weight, max_weight)
            graph[i][j] = weight
            graph[j][i] = weight
    return graph

def save_graph_to_file(graph, filename):
    with open(filename, 'w') as f:
        length = len(graph)
        f.write(str(length)+"\n")
        for row in graph:
            f.write(" ".join(str(x) for x in row) + "\n")

# Przykład użycia
n = int(sys.argv[1])
graph = generate_graph(n)
filename = "dane"+str(n)+".txt"
save_graph_to_file(graph, filename)

print("Zapisano dane do pliku "+filename)
