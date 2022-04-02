#include "bfs.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitset.h"
#include "error.h"
#include "grid.h"
#include "stdint.h"

// Return the distance between start and stop using the board and the grid
// settings, or SIZE_MAX if there is no clear path. Writes over the
// board object.
size_t bfs(grid_t grid, bitset_t board, size_t start, size_t stop) {
  bitset_t visited = board;  // To save memory.

  size_t layer_i = 0;
  vector_t *layers;
  ALLOC_N(layers, 2);
#define THIS_LAYER layers[layer_i % 2]
#define NEXT_LAYER layers[(layer_i + 1) % 2]
  THIS_LAYER = new_vector(), NEXT_LAYER = new_vector();

  vector_append(THIS_LAYER, start);
  for (;;) {
    if (vector_size(THIS_LAYER) == 0) return SIZE_MAX;
    assert(vector_size(NEXT_LAYER) == 0);

    while (vector_size(THIS_LAYER) > 0) {
      size_t a = vector_pop(THIS_LAYER);

      if (a == stop) return layer_i;

      for (size_t ax = 0; ax < grid_k(grid); ++ax) {
        for (int dt = -1; dt <= 1; dt += 2) {
          size_t b = grid_move(grid, a, ax, dt);
          if (b == SIZE_MAX || bitset_get(visited, b)) continue;
          bitset_set(visited, b, true);
          vector_append(NEXT_LAYER, b);
        }
      }
    }
    ++layer_i;
  }
}
