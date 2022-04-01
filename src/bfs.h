#include <stdio.h>
#include <stdlib.h>

#include "bitset.h"
#include "error.h"
#include "grid.h"
#include "stdint.h"

size_t bfs(grid_t grid, bitset_t board, size_t start, size_t stop);
