#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "error.h"
#include <stdlib.h>

struct _bitset_impl {
  size_t n_bits, n_cells;
  unsigned *cells;
};
typedef struct _bitset_impl *bitset_t;
const size_t BITS_PER_CELL = 8UL * sizeof(unsigned);


// Creates a new bitset, filled with n zeroes. Returns the object, or NULL on
// alloc failure.
bitset_t new_bitset(size_t n) {
  bitset_t this;
  ALLOC(this);

  this->n_bits = n, this->n_cells = n / BITS_PER_CELL + 1;

  ALLOC_N(this->cells, this->n_cells);

  return this;
}

// i-th bit is the J-th bit in the I-th cell.
#define I (i / BITS_PER_CELL)
#define J (i % BITS_PER_CELL)

// Returns the i-th bit. Never fails.
bool bitset_get(bitset_t o, size_t i) {
  assert(i < o->n_bits);
  return o->cells[I] & (1U << J) ? true : false;
}

// Sets the i-th bit. Never fails.
void bitset_set(bitset_t o, size_t i, bool x) {
  assert(i < o->n_bits);
  o->cells[I] = (o->cells[I] & ~(1UL << J)) | (x << J);
}

void bitset_print(bitset_t o, size_t n) {
  printf("{");
  for (size_t i = n; i-- > 0;) {
    printf("%i", bitset_get(o, i));
  }
  printf("}");
}