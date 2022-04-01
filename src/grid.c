
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

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

  this->k = vector_size(n);

  this->n = vector_copy(n);

  this->n_pref = new_vector();
  vector_append(this->n_pref, 1);
  for (size_t i = 1; i <= this->k; ++i) {
    vector_append(this->n_pref,
                  vector_get(this->n_pref, i - 1) * vector_get(this->n, i - 1));
  }

  return this;
}

size_t grid_rank(grid_t grid, vector_t vec) {
  size_t x = 0;
  for (size_t i = 0; i < vector_size(vec); ++i) {
    x += (vector_get(vec, i) - 1) * vector_get(grid->n_pref, i);
  }
  return x;
}

size_t grid_unrank(grid_t grid, size_t id, size_t axis) {
  return (id % vector_get(grid->n_pref, axis + 1)) /
         vector_get(grid->n_pref, axis);
}

size_t grid_move(grid_t grid, size_t id, size_t axis, int delta) {
  assert(delta == -1 || delta == +1);
  size_t x = grid_unrank(grid, id, axis);
  if ((x == 0 && delta == -1)) return SIZE_MAX;
  // if ((x == vector_get(grid->n, axis) - 1 && delta == 1)) return SIZE_MAX;
  return id + delta * vector_get(grid->n_pref, axis);
}
