#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// PS: I put all the documentation-like comments next to the implementations,
// not the definitions, because most editors find them anyway.
#include "arithmetic.h"
#include "bfs.h"
#include "bitset.h"
#include "error.h"
#include "grid.h"
#include "parser.h"
#include "vector.h"

int main() {
  grid_t grid = parser_eat_grid();
  parser_eat_newline();
  size_t start = parser_eat_position(grid);
  parser_eat_newline();
  size_t stop = parser_eat_position(grid);
  parser_eat_newline();
  bitset_t board = parser_eat_board(grid_volume(grid));

  // The tests provided require the error to be on these lines in these cases.
  // Some tests disagree.
  parser_error_retro_if(2, bitset_get(board, start));
  parser_error_retro_if(3, bitset_get(board, stop));

  parser_eat_eof();

  size_t ans = bfs(grid, board, start, stop);

  if (ans == SIZE_MAX)
    printf("NO WAY\n");
  else
    printf("%zu\n", ans);

  clean_up();
  return 0;
}