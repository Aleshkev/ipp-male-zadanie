
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitset.h"
#include "io.h"

#define ABORT(N)                    \
  {                                 \
    fprintf(stderr, "ERROR %i", N); \
    return 1;                       \
  }

int main() {

  size_t n_dimensions, *dimensions;

  bool malformed = 0, error = 0;
  dimensions = eat_line_1(&n_dimensions, &malformed, &error);
  if (malformed) ABORT(1);
  if (dimensions == NULL) ABORT(0);
  if (ferror(stdin) || error) {
    free(dimensions);
    ABORT(0);
  }

  size_t volume = 1;
  for (size_t i = 0; i < n_dimensions; ++i) volume *= dimensions[i];

  size_t *start_pos, *end_pos;
  start_pos =
      eat_line_2_or_3(n_dimensions, dimensions, volume, &malformed, &error);
  end_pos =
      eat_line_2_or_3(n_dimensions, dimensions, volume, &malformed, &error);
  if (start_pos == NULL || end_pos == NULL || ferror(stdin) || error) {
    free(dimensions);
    ABORT(0);
  }

  unsigned *board;
  board = eat_line_4(n_dimensions, dimensions, volume, &malformed, &error);

  // Line 2 & 3.

  printf("[%i %i]", malformed, error);

  for (size_t i = 0; i < n_dimensions; ++i) {
    printf("%zu ", dimensions[i]);
  }
  printf("\n");
  for (size_t i = 0; i < n_dimensions; ++i) {
    printf("%zu ", start_pos[i]);
  }
  printf("\n");
  for (size_t i = 0; i < n_dimensions; ++i) {
    printf("%zu ", end_pos[i]);
  }
  printf("\n");

  print_bitset(board, volume);

  free(dimensions);
  return 0;
}
