#include "bitset.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"


struct _bitset_impl {
  size_t n_bits, n_cells;
  unsigned *cells;
};
typedef struct _bitset_impl *bitset_t;
const size_t BITS_PER_CELL = 8UL * sizeof(unsigned);

// Create a bitset.
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

// Read the i-th bit.
bool bitset_get(bitset_t this, size_t i) {
  assert(i < this->n_bits);
  return this->cells[I] & (1U << J) ? true : false;
}

// Set the i-th bit.
void bitset_set(bitset_t this, size_t i, bool x) {
  assert(i < this->n_bits);
  this->cells[I] = (this->cells[I] & ~(1U << J)) | ((unsigned)x << J);
}

// Swap the values of the i-th and j-th bits.
void bitset_swap(bitset_t this, size_t i, size_t j) {
  bool a = bitset_get(this, i), b = bitset_get(this, j);
  bitset_set(this, i, b), bitset_set(this, j, a);
}

// Print the bitset. 0-th element is to the right.
void bitset_print(bitset_t this) {
  printf("(bitset_t)");
  for (size_t i = this->n_bits; i-- > 0;) {
    printf("%i", bitset_get(this, i));
  }
}