#define _GNU_SOURCE

#include <stdio.h>
#include <time.h>
#include "graph.h"
#include "math_funcs.h"
#include "edge_variations.h"
#include "error_codes.h"

#define MAX_ADDITIONS 3

unsigned long int calc_elapsed_time(const struct timespec *beg,
                         const struct timespec *end) {
    return ((end->tv_sec - beg->tv_sec) * 1000 * 1000 * 1000 +
        (end->tv_nsec - beg->tv_nsec));
}

static void fill_every_void_edge(graph_matrix *graph)
{
    for (size_t i = 0; i < graph->n; i++)
    {
        for (size_t j = 0; j < graph->n; j++)
        {
            if (graph->adj_matrix[i][j] == -1)
                graph->adj_matrix[i][j] = 2;
        }
    }
}


int main(int argc, char **argv)
{
    if (argc > 2)
    {
        printf("Incorrect params!\n");
        return ERR_ARGS;
    }
    
    struct timespec start, end;
    unsigned long int calc_time = 0;

    int rc = OK, beg, max_path;
    graph_matrix graph;
    graph.adj_matrix = NULL;
    struct edge_arr_t accesable_edges;
    accesable_edges.variations = NULL;
    FILE *f = NULL;

    if (argc == 1)
        f = stdin;
    else
    {
        f = fopen(argv[1], "r");
        if (!f)
            rc = ERR_OPEN;
    }

    if (rc == OK)
        rc = input_graph(f, &graph);
    if (rc == OK)
    {
        printf("Memory for graph: %lu bytes\n", sizeof(graph) +
         graph.n * sizeof(int*) + graph.n * graph.n * sizeof(int));
        if (f == stdin)
            printf("Введите начальный город (1, %lu): ", graph.n);
        if (fscanf(f, "%d", &beg) != 1)
            rc = ERR_IO;
        else
            rc = (beg < 1 || beg > (int) graph.n) ? ERR_RANGE : OK;
    }
    if (rc == OK)
    {
        if (f == stdin)
            printf("Введите максимально допустимую длину пути: ");
        if (fscanf(f, "%d", &max_path) != 1)
            rc = ERR_IO;
        else
            rc = max_path < 1 ? ERR_RANGE : OK;
    }
    
    if (rc == OK)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        size_t void_edges = count_void_edges(&graph);
        if (void_edges <= MAX_ADDITIONS)
        {
            fill_every_void_edge(&graph);
            
            clock_gettime(CLOCK_MONOTONIC_RAW, &end);
            calc_time = calc_elapsed_time(&start, &end);
            printf("TRUE\n");
        }
        else
        {
            size_t additions_combines = count_combinations(void_edges, MAX_ADDITIONS);
            rc = fill_access_arr(&graph, &accesable_edges, additions_combines);
            if (rc == OK)
            {
                clock_gettime(CLOCK_MONOTONIC_RAW, &end);
                calc_time = calc_elapsed_time(&start, &end);
                
                printf("Memory for void edges: %lu bytes\n", sizeof(accesable_edges) +
                 accesable_edges.count * sizeof(pair[3]));
                clock_gettime(CLOCK_MONOTONIC_RAW, &start);

                rc = identify_path(&graph, beg-1, &accesable_edges, max_path);
                if (rc == OK || rc == NO_TPATH)
                {
                    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
                    calc_time += calc_elapsed_time(&start, &end);
                    
                    printf("%s\n", (rc == OK) ? "TRUE" : "FALSE");
                    rc = OK;
                }
            }
        }
    }
    
    if (rc == OK)
    {
        export_graph(&graph);
        printf("Calculation time: %lu ns\n", calc_time);
    }

    if (f && f != stdin)
        fclose(f);
    free_graph(&graph);
    free_void_edges(&accesable_edges);
    
    if (rc == ERR_OPEN)
        printf("Ошибка открытия файла\n");
    else if (rc == ERR_IO)
        printf("Неверный формат данных\n");
    else if (rc == ERR_RANGE)
        printf("Неверный интервал ввода\n");
    else if (rc == ERR_MEM)
        printf("Ошибка выделения памяти\n");
    return rc;
}