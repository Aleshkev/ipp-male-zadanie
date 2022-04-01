
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "error.h"
#include "vector.h"

struct _grid_impl {
  size_t k;
  vector_t n, n_pref;
};
typedef struct _grid_impl *grid_t;

grid_t new_grid(vector_t n) {
  grid_t this;
  ALLOC(this);

  this->n = vector_copy(n);
  this->k = vector_size(this->n);

  this->n_pref = new_vector();
  vector_append(this->n_pref, 1);
  for (size_t i = 1; i <= this->k; ++i) {
    vector_append(this->n_pref,
                  vector_get(this->n_pref, i - 1) * vector_get(this->n, i - 1));
  }

  return this;
}

size_t grid_rank(grid_t this, vector_t vec) {
  size_t x = 0;
  for (size_t i = 0; i < vector_size(vec); ++i) {
    if (vector_get(vec, i) > vector_get(this->n, i)) return SIZE_MAX;
    x += (vector_get(vec, i) - 1) * vector_get(this->n_pref, i);
  }
  return x;
}

size_t grid_unrank(grid_t this, size_t id, size_t axis) {
  return (id % vector_get(this->n_pref, axis + 1)) /
         vector_get(this->n_pref, axis);
}

size_t grid_move(grid_t this, size_t id, size_t axis, int delta) {
  assert(delta == -1 || delta == +1);

  size_t x = grid_unrank(this, id, axis);
  if (x == 0 && delta == -1) return SIZE_MAX;
  if (x == vector_get(this->n, axis) - 1 && delta == 1) return SIZE_MAX;

  size_t id2 = id + delta * vector_get(this->n_pref, axis);

  // printf("%zu -> %zu\n", id, id2);

  return id2;
}

size_t grid_volume(grid_t this) { return vector_get(this->n_pref, this->k); }
size_t grid_k(grid_t this) { return this->k; }
