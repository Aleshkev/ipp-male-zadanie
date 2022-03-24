
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const size_t BITS_PER_CELL = 8UL * sizeof(unsigned);

struct _bitset_impl {
  size_t n_bits, n_cells;
  unsigned *cells;
};
typedef struct _bitset_impl *bitset_t;

// void print_bitset(unsigned *t, size_t n);

// Creates a new bitset, filled with n zeroes. NULL on alloc failure.
bitset_t bitset_calloc(size_t n) {
  bitset_t o = malloc(sizeof *o);
  if (o == NULL) return NULL;

  o->n_bits = n, o->n_cells = n / BITS_PER_CELL + 1;

  o->cells = calloc(o->n_cells, sizeof *o->cells);
  if (o->cells == NULL) return free(o), NULL;

  return o;
}

// Frees.
void bitset_free(bitset_t o) {
  free(o->cells), o->cells = NULL;
  free(o);
}

// i-th bit is the J-th bit in the I-th cell.
#define I (i / BITS_PER_CELL)
#define J (i % BITS_PER_CELL)

// Returns the i-th bit.
bool bitset_get(bitset_t o, size_t i) {
  assert(i < o->n_bits);
  return o->cells[I] & (1U << J) ? true : false;
}

// Sets the i-th bit.
void bitset_set(bitset_t o, size_t i, bool x) {
  assert(i < o->n_bits);
  (o->cells[I] & ~(1UL << J)) | (x << J);
}

void print_bitset(unsigned *t, size_t n) {
  //   for (size_t i = 0; i < n / 32; ++i) {
  //     printf("%u", t[i]);
  //   }
  //   printf(">\n");
  for (size_t i = 0; i < n; ++i) {
    printf("%i", bitset_get(t, i));
  }
  printf("\n");
}
