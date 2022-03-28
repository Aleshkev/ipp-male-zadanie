#include <stdbool.h>
#include <stdlib.h>

struct _bitset_impl;
typedef struct _bitset_impl *bitset_t;

bitset_t bitset_calloc(size_t n);
void bitset_free(bitset_t o);
bool bitset_get(bitset_t o, size_t i);
void bitset_set(bitset_t o, size_t i, bool x);
void bitset_print(bitset_t o, size_t n);
