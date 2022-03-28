
#include "vector.h"

struct _grid_impl;
typedef struct _grid_impl *grid_t;

grid_t new_grid(vector_t n);
void grid_free(grid_t grid);
size_t grid_rank(grid_t grid, vector_t vec);
size_t grid_unrank(grid_t grid, size_t id, size_t axis);
size_t grid_move(grid_t grid, size_t id, size_t axis, int delta);
