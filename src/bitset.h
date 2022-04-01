#include <stdbool.h>
#include <stdlib.h>

struct _bitset_impl;
typedef struct _bitset_impl *bitset_t;

bitset_t new_bitset(size_t n);
void bitset_free(bitset_t this);
bool bitset_get(bitset_t this, size_t i);
void bitset_set(bitset_t this, size_t i, bool x);
void bitset_swap(bitset_t this, size_t i, size_t j);
void bitset_print(bitset_t this);
