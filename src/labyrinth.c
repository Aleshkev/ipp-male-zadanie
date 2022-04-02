#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "arithmetic.h"
#include "bfs.h"
#include "bitset.h"
#include "error.h"
#include "grid.h"
#include "parser.h"
#include "vector.h"

int main() {
  vector_t dimensions = parser_eat_vector(1);
  CHECK_INPUT(1, vector_size(dimensions) == 0);
  parser_eat_newline();

  grid_t grid = new_grid(dimensions);

  vector_t start_vec = parser_eat_vector(2);
  size_t start = grid_rank(grid, start_vec, 2);
  parser_eat_newline();

  vector_t stop_vec = parser_eat_vector(3);
  size_t stop = grid_rank(grid, stop_vec, 3);
  parser_eat_newline();

  bitset_t board = parser_eat_board(grid_volume(grid));

  CHECK_INPUT(2, bitset_get(board, start));
  CHECK_INPUT(3, bitset_get(board, stop));

  size_t ans = bfs(grid, board, start, stop);
  parser_eat_eof();

  if (ans == SIZE_MAX)
    printf("NO WAY\n");
  else
    printf("%zu\n", ans);

  clean_up();
  return 0;
}