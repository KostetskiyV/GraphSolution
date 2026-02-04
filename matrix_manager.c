#include <stdlib.h>
#include "matrix_manager.h"
#include "error_codes.h"

int alloc_matrix(int ***matrix, size_t n)
{
    *matrix = calloc(n, sizeof(int*));
    if (!*matrix)
        return ERR_MEM;

    for (size_t i = 0; i < n; i++)
    {
        (*matrix)[i] = malloc(n * sizeof(int));
        if (!(*matrix)[i])
        {
            free_matrix(matrix, i);
            return ERR_MEM;
        }

        for (size_t j = 0; j < n; j++)
            (*matrix)[i][j] = -1;
        (*matrix)[i][i] = 0;
    }
    return OK;
}

void free_matrix(int ***matrix, size_t n)
{
    if (!*matrix)
        return;

    for (size_t i = 0; i < n; i++)
        free((*matrix)[i]);
    free(*matrix);
    *matrix = NULL;
}
