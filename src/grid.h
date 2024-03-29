
#include "vector.h"

struct _grid_impl;
typedef struct _grid_impl *grid_t;

grid_t new_grid(vector_t n);
void grid_free(grid_t this);
size_t grid_rank(grid_t this, vector_t vec);
size_t grid_unrank(grid_t this, size_t id, size_t axis);
size_t grid_move(grid_t this, size_t id, size_t axis, int delta);
size_t grid_volume(grid_t this);
size_t grid_k(grid_t this);