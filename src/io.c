
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitset.h"
#include "vector.h"

#define explain(...) fprintf(stderr, __VA_ARGS__)

char peekchar() {
  char c = getchar();
  ungetc(c, stdin);
  return c;
}

char fmtchar(char c) { return (c == '\n' ? '^' : c); }

int hexdigit(char c) {
  return ('0' <= c && c <= '9'                     ? c - '0'
          : 'a' <= tolower(c) && tolower(c) <= 'f' ? tolower(c) - 'a'
                                                   : -1);
}

// The error code is global, because error state is global. The program can not
// continue executing.

size_t eat_unsigned(size_t min, size_t max, char *error) {
  if (!isalnum(peekchar())) error = "expected a digit (to begin a number)";

  size_t x = 69;
  if (scanf("%zu", &x) < 1) error = "failed to read a number";

  if (x < min || x > max) {
    explain("number not between %zu and %zu\n", min, max),
        *malformed = *error = true;
  }
  return x;
}

void eat_spaces(size_t min, bool *malformed, bool *error) {
  size_t n = 0;
  for (; peekchar() == ' ' || peekchar() == '\t'; getchar()) ++n;
  if (n < min) {
    explain("expected at least %zu space(s)\n", min);
    *malformed = *error = true;
  }
}

void eat_trailing_whitespace(bool *malformed, bool *error) {
  for (; isspace(peekchar()); getchar())
    ;
  if (peekchar() != EOF) {
    explain("expected end of input\n");
    *malformed = *error = true;
  }
}

void eat_symbol(char s, bool *malformed, bool *error) {
  if (peekchar() != s) {
    explain("expected '%c', read '%c'\n", fmtchar(s), fmtchar(peekchar()));
    *malformed = *error = true;
  }
  getchar();
}

vector_t eat_line_1(bool *malformed, bool *error) {
  vector_t dimensions = vector_calloc();
  if (dimensions == NULL) return NULL;

  for (size_t i = 0;; ++i) {
    eat_spaces(0, malformed, error);

    if (*error || peekchar() == '\n' || peekchar() == EOF) break;

    size_t x = eat_unsigned(1, SIZE_MAX, malformed, error);
    dimensions = vector_append(dimensions, x);
    if (dimensions == NULL) return NULL;
  }
  eat_symbol('\n', malformed, error);
  if (vector_size(dimensions) < 1) {
    explain("there must be at least one dimension\n");
    *malformed = true;
  }

  return dimensions;
}

size_t *eat_line_2_or_3(vector_t dimensions, bool *malformed, bool *error) {
  size_t *position = calloc(vector_size(dimensions), sizeof(size_t));
  if (position == NULL) return NULL;

  for (size_t i = 0; i < vector_size(dimensions); ++i) {
    eat_spaces((i == 0 ? 0 : 1), malformed, error);
    position[i] = eat_unsigned(1, vector_get(dimensions, i), malformed, error);
  }
  eat_spaces(0, malformed, error);
  eat_symbol('\n', malformed, error);
  return position;
}

unsigned *eat_line_4(size_t n_dimensions, const size_t *dimensions,
                     size_t volume, bool *malformed, bool *error) {
  bitset_t board = bitset_calloc(volume);
  if (board == NULL) return NULL;

  if (peekchar() == 'R') {
    eat_symbol('R', malformed, error);
    size_t d[5];
    for (size_t i = 0; i < 5; ++i) {
      eat_spaces((i == 0 ? 0 : 1), malformed, error);
      d[i] = eat_unsigned((i == 2 ? 1 : 0), UINT32_MAX, malformed, error);
    }
    size_t a = d[0], b = d[1], m = d[2], r = d[3], s0 = d[4];
  } else {
    eat_symbol('0', malformed, error);
    eat_symbol('x', malformed, error);

    bool leading = true;
    for (size_t i = 0; i < volume; ++i) {
      if (hexdigit(peekchar()) == -1) break;
      int digit = hexdigit(getchar());
      for (size_t k = 4; k-- > 0;) {
        bool bit = digit & (1 << k);
        if (bit) leading = false;
        if (leading) continue;
        if (i == volume) {
          explain("too many bits");
          *malformed = *error = true;
        } else {
          bitset_set(board, i, bit);
        }
      }
    }
  }
  eat_spaces(0, malformed, error);
  if (peekchar() == '\n') eat_symbol('\n', malformed, error);

  return board;
}
