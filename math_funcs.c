#include "math_funcs.h"

unsigned long long fact(unsigned int num)
{
    if (num == 0)
        return 1;
    unsigned long long res = 1;
    for (unsigned int i = 2; i <= num; i++)
        res *= i;
    return res;
}

int count_combinations(unsigned int n, unsigned int k)
{
    if (k > n)
        return 0;
    return (int) (fact(n) / fact(k) / fact(n-k));
}
