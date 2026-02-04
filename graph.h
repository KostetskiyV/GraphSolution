#ifndef GRAPH_H__
#define GRAPH_H__

#include <stddef.h>
#include "edge_variations.h"

typedef struct
{
    size_t n;
    int **adj_matrix;
} graph_matrix;

int input_graph(FILE *f, graph_matrix *graph);
void free_graph(graph_matrix *graph);
void export_graph(const graph_matrix *graph);

int count_void_edges(const graph_matrix *graph);
int fill_access_arr(const graph_matrix *graph, struct edge_arr_t *accesable_edges, size_t additions_combines);
void free_void_edges(struct edge_arr_t *accesable_edges);

int dijkstra_calc(const graph_matrix *graph, size_t beg, int *lens);
int identify_path(graph_matrix *graph, int beg, struct edge_arr_t *accesable_edges, int max_len);

#endif