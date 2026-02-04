#ifndef EDGE_VARIATIONS_H__
#define EDGE_VARIATIONS_H__

typedef struct
{
    size_t from;
    size_t to;
} pair;

struct edge_arr_t
{
    size_t count;
    pair (*variations)[3];
};

#endif
