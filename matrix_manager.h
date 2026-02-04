#ifndef MATRIX_MANAGER_H__
#define MATRIX_MANAGER_H__

#include <stddef.h>

int alloc_matrix(int ***matrix, size_t n);
void free_matrix(int ***matrix, size_t n);

#endif
