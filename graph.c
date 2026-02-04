#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "graph.h"
#include "matrix_manager.h"
#include "error_codes.h"

#define OUT_PATH "data/graph.gv"

static bool accept_lens(const int *lens, size_t lens_n, int max_len)
{
    for (size_t i = 0; i < lens_n; i++)
    {
        if (lens[i] > max_len || lens[i] == -1)
            return false;
    }
    return true;
}

static int find_closest_unreviewed(const int *lens, const bool *reviewed, size_t n)
{
    int min = -1;
    for (size_t i = 0; i < n; i++)
    {
        if ((min == -1 || lens[min] > lens[i]) && !reviewed[i])
            min = i;
    }
    return min;
}


int input_graph(FILE *f, graph_matrix *graph)
{
    graph->adj_matrix = NULL;
    if (f == stdin)
        printf("Введите количество городов: ");
    if (fscanf(f, "%lu", &(graph->n)) != 1)
        return ERR_IO;
    if (graph->n <= 0)
        return ERR_RANGE;
    int rc = alloc_matrix(&(graph->adj_matrix), graph->n);
    if (rc != OK)
        return rc;

    int n;
    if (f == stdin)
        printf("Введите количество односторонних дорог: ");
    if (fscanf(f, "%d", &n) != 1)
        return ERR_IO;
    if (n < 0 || n > (int) (graph->n * (graph->n - 1)))
        return ERR_RANGE;

    if (f == stdin)
        printf("Введите %d пар [город отправления] - [город назначения]:\n", n);
    for (size_t i = 0; (int) i < n; i++)
    {
        int from, to;
        if (fscanf(f, "%d%d", &from, &to) != 2)
            return ERR_IO;
        if (from < 1 || from > (int) graph->n || from < 1 || from > (int) graph->n || from == to || graph->adj_matrix[from-1][to-1] != -1)
            return ERR_RANGE;

        graph->adj_matrix[from-1][to-1] = 1;
    }
    return OK;
}

int count_void_edges(const graph_matrix *graph)
{
    int res = 0;
    for (size_t i = 0; i < graph->n; i++)
    {
        for (size_t j = 0; j < graph->n; j++)
        {
            if (graph->adj_matrix[i][j] == -1)
                res++;
        }
    }
    return res;
}

void free_graph(graph_matrix *graph)
{
    free_matrix(&(graph->adj_matrix), graph->n);
    graph->n = 0;
}

void export_graph(const graph_matrix *graph)
{
    if (!graph->adj_matrix)
        return;

    FILE *fout = fopen(OUT_PATH, "w");
    if (fout)
        fprintf(fout, "digraph map {\n");
    for (size_t i = 0; i < graph->n; i++)
    {
        for (size_t j = 0; j < graph->n; j++)
        {
            if (graph->adj_matrix[i][j] == -1)
                printf("-\t");
            else
            {
                if (fout && i != j)
                    fprintf(fout, "%lu -> %lu [color=%s];\n", i + 1, j + 1, graph->adj_matrix[i][j] == 1 ? "black" : "red");
                printf("%d\t", graph->adj_matrix[i][j]);
            }
        }
        printf("\n");
    }
    if (fout)
    {
        fprintf(fout, "}\n");
        fclose(fout);
    }
}

int fill_access_arr(const graph_matrix *graph, struct edge_arr_t *accesable_edges, size_t additions_combines)
{
    accesable_edges->count = additions_combines;
    accesable_edges->variations = calloc(additions_combines, sizeof(pair[3]));
    if (!accesable_edges->variations)
        return ERR_MEM;

    size_t cur_combination = 0, cur_level = 0;
    pair points[3];
    for (size_t i = 0; cur_combination < additions_combines; i++)
    {
        for (size_t j = 0; j < graph->n && cur_combination < additions_combines; j++)
        {
            if (graph->adj_matrix[i][j] == -1)
            {
                points[cur_level].from = i;
                points[cur_level].to = j;
                cur_level++;
                if (cur_level == 3)
                {
                    accesable_edges->variations[cur_combination][0] = points[0];
                    accesable_edges->variations[cur_combination][1] = points[1];
                    accesable_edges->variations[cur_combination][2] = points[2];
                    cur_combination++;
                    cur_level--;
                }
            }

            if (i == graph->n - 1 && j == graph->n - 1)
            {
                cur_level--;
                i = points[cur_level].from;
                j = points[cur_level].to;
            }
        }
    }
    return OK;
}

void free_void_edges(struct edge_arr_t *accesable_edges)
{
    free(accesable_edges->variations);
}

int dijkstra_calc(const graph_matrix *graph, size_t beg, int *lens)
{
    bool *reviewed = calloc(graph->n, sizeof(bool));
    if (!reviewed)
        return ERR_MEM;

    int init_val = graph->n * 10;
    for (size_t i = 0; i < graph->n; i++)
        lens[i] = (i == beg) ? 0 : init_val;
    
    int cur = find_closest_unreviewed(lens, reviewed, graph->n);
    while (cur != -1)
    {
        reviewed[cur] = true;
        for (size_t j = 0; j < graph->n; j++)
        {
            int jcur_len = graph->adj_matrix[cur][j];
            if (jcur_len != -1)
            {
                int sumlen = lens[cur] + jcur_len;
                if (sumlen < lens[j])
                    lens[j] = sumlen;
            }
        }
        cur = find_closest_unreviewed(lens, reviewed, graph->n);
    }
    
    for (size_t i = 0; i < graph->n; i++)
        lens[i] = (lens[i] >= init_val) ? -1 : lens[i];
    return OK;
}

int identify_path(graph_matrix *graph, int beg, struct edge_arr_t *accesable_edges, int max_len)
{
    int *lens = calloc(graph->n, sizeof(int));
    if (!lens)
        return ERR_MEM;

    for (size_t var = 0; var < accesable_edges->count; var++)
    {
        pair road1 = accesable_edges->variations[var][0],
         road2 = accesable_edges->variations[var][1],
         road3 = accesable_edges->variations[var][2];
        
        graph->adj_matrix[road1.from][road1.to] = 1;
        graph->adj_matrix[road2.from][road2.to] = 1;
        graph->adj_matrix[road3.from][road3.to] = 1;

        if (dijkstra_calc(graph, beg, lens) != OK)
        {
            free(lens);
            return ERR_MEM;
        }
        if (accept_lens(lens, graph->n, max_len))
        {
            graph->adj_matrix[road1.from][road1.to] = 2;
            graph->adj_matrix[road2.from][road2.to] = 2;
            graph->adj_matrix[road3.from][road3.to] = 2;
            free(lens);
            return OK;
        }

        graph->adj_matrix[road1.from][road1.to] = -1;
        graph->adj_matrix[road2.from][road2.to] = -1;
        graph->adj_matrix[road3.from][road3.to] = -1;
    }
    free(lens);
    return NO_TPATH;
}
