#include "grid.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "arithmetic.h"
#include "error.h"
#include "parser.h"
#include "vector.h"

struct _grid_impl {
  size_t k;
  vector_t n, n_pref;
};
typedef struct _grid_impl *grid_t;

// A grid is an abstract object that stores information about and operates on
// the dimensions of the board. It doesn't really need to exist, but it does,
// and the code looks a bit tidier.
grid_t new_grid(vector_t n) {
  grid_t this;
  ALLOC(this);

  // Yes, this is a redundant copy of the vector. We're wasting dozens of bytes
  // here.
  this->n = vector_copy(n);
  this->k = vector_size(this->n);
  parser_error_if(this->k == 0);

  this->n_pref = new_vector();
  vector_append(this->n_pref, 1);
  bool overflow = false;
  for (size_t i = 1; i <= this->k; ++i) {
    vector_append(this->n_pref, mulcap(vector_get(this->n_pref, i - 1),
                                       vector_get(this->n, i - 1), &overflow));
  }

  // This will throw ERROR 0 if the volume of the grid is larger than SIZE_MAX.
  // This is for consistency with the fact that volume slightly lesser than
  // SIZE_MAX would fail with ERROR 0 because of insufficient memory.
  parser_error_retro_if(0, overflow);
  parser_error_if(grid_volume(this) == 0);

  return this;
}

// Convert k numbers into a single number, unambiguously representing the
// position.
size_t grid_rank(grid_t this, vector_t vec) {
  parser_error_if(vector_size(vec) != this->k);
  size_t x = 0;
  for (size_t i = 0; i < vector_size(vec); ++i) {
    size_t coord = vector_get(vec, i);
    parser_error_if(coord < 1);
    parser_error_if(coord > vector_get(this->n, i));

    x += (vector_get(vec, i) - 1) * vector_get(this->n_pref, i);
  }
  return x;
}

// Reverse grid_rank(), along one of the axes. (Get one of the original
// coordinates.)
size_t grid_unrank(grid_t this, size_t id, size_t axis) {
  return (id % vector_get(this->n_pref, axis + 1)) /
         vector_get(this->n_pref, axis);
}

// Return the id of the coordinates represented by the id, shifted by delta
// along the axis, or SIZE_MAX if the new coordinates would fall outside the
// grid bounds.
size_t grid_move(grid_t this, size_t id, size_t axis, int delta) {
  assert(delta == -1 || delta == +1);

  size_t x = grid_unrank(this, id, axis);
  if (x == 0 && delta == -1) return SIZE_MAX;
  if (x == vector_get(this->n, axis) - 1 && delta == 1) return SIZE_MAX;

  return id + delta * vector_get(this->n_pref, axis);
}

// The total number of fields the board has.
size_t grid_volume(grid_t this) { return vector_get(this->n_pref, this->k); }

// The number of dimensions the board has.
size_t grid_k(grid_t this) { return this->k; }
