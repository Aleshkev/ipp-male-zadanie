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
  parser_t parser = new_parser();

  vector_t dimensions = NULL, start_pos = NULL, stop_pos = NULL;
  dimensions = parser_read_vector(parser);
  // CHECK_INPUT(1, !parser_is_input_ok(parser));
  // vector_print(dimensions), printf("\n");
  CHECK_INPUT(1, vector_size(dimensions) == 0);

  grid_t grid = new_grid(dimensions);

  // printf("%zu\n", grid_volume(grid));
  CHECK_INPUT(1, grid_volume(grid) == 0);
  CHECK_INPUT(0, grid_volume(grid) == SIZE_MAX);

  start_pos = parser_read_vector(parser);
  // CHECK_INPUT(2, !parser_is_input_ok(parser));
  // vector_print(start_pos), printf("\n");
  CHECK_INPUT(2, vector_size(start_pos) != grid_k(grid));
  

  stop_pos = parser_read_vector(parser);
  // CHECK_INPUT(3, !parser_is_input_ok(parser));
  // vector_print(stop_pos), printf("\n");
  CHECK_INPUT(3, vector_size(stop_pos) != grid_k(grid));

  bitset_t board = parser_read_board(parser, grid_volume(grid));
  // CHECK_INPUT(4, !parser_is_input_ok(parser));
  // bitset_print(board), printf("\n");

  size_t ans = bfs(grid, board, grid_rank(grid, start_pos), grid_rank(grid, stop_pos));

  if (ans == SIZE_MAX)
    printf("NO WAY\n");
  else
    printf("%zu\n", ans);

  clean_up();
  return 0;
}