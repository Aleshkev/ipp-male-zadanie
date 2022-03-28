#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "arithmetic.h"
#include "bitset.h"
#include "error.h"
#include "grid.h"
#include "parser.h"
#include "vector.h"

int main() {
  parser_t parser = new_parser();

  vector_t dimensions = NULL, start_pos = NULL, stop_pos = NULL;
  dimensions = parser_read_vector(parser);
  CHECK_INPUT(1, !parser_is_input_ok(parser));
  vector_print(dimensions), printf("\n");
  CHECK_INPUT(1, vector_size(dimensions) == 0);

  grid_t grid = new_grid(dimensions);

  size_t n_dimensions = vector_size(dimensions),
         volume = vector_get(dimensions, 0);
  for (size_t i = 1; i < n_dimensions; ++i)
    volume = mulcap(volume, vector_get(dimensions, i));

  printf("%zu\n", volume);
  CHECK_INPUT(1, volume == 0);
  CHECK_INPUT(0, volume == SIZE_MAX);

  start_pos = parser_read_vector(parser);
  CHECK_INPUT(2, !parser_is_input_ok(parser));
  vector_print(start_pos), printf("\n");
  CHECK_INPUT(2, vector_size(start_pos) != n_dimensions);

  stop_pos = parser_read_vector(parser);
  CHECK_INPUT(3, !parser_is_input_ok(parser));
  vector_print(stop_pos), printf("\n");
  CHECK_INPUT(3, vector_size(stop_pos) != n_dimensions);

  bitset_t board = parser_read_bitset(parser, volume);
  CHECK_INPUT(4, !parser_is_input_ok(parser));
  bitset_print(board, volume), printf("\n");

  parser_eat_trailing_whitespace(parser);
  CHECK_INPUT(5, !parser_is_input_ok(parser));

  clean_up();
  return 0;
}