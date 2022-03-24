
#include <stdbool.h>
#include <stdio.h>

typedef bitset_t;

bitset_t bitset_calloc(size_t n);
void bitset_free(bitset_t b);
bool bitset_get(bitset_t b, size_t i);
void bitset_set(bitset_t b, size_t i, bool x);
void print_bitset(bitset_t b, size_t n);