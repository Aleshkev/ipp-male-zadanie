#include "parser.h"

#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitset.h"
#include "error.h"
#include "vector.h"

struct _parser_impl {
  bool system_ok, input_ok;
};
typedef struct _parser_impl *parser_t;

parser_t new_parser() {
  parser_t p;
  ALLOC(p);
  return p;
}

int peekchar() {
  int c = getchar();
  ungetc(c, stdin);
  return c;
}

char fmtchar(char c) { return (c == '\n' ? '^' : c); }

int hexdigit(int c) {
  c = tolower(c);
  return '0' <= c && c <= '9'   ? c - '0'  //
         : 'a' <= c && c <= 'f' ? 10 + c - 'a'
                                : -1;
}

#define _STR(X) #X
#define STR(X) _STR(X)
const char SIZE_MAX_STR[] = STR(SIZE_MAX);

// void reversec(char *start, char *stop) {
//   for (size_t i = 0; start + 2 * i < stop; ++i) {
//     swap(start + i, stop - i - 1);
//   }
// }

// [0-9]+
size_t parser_eat_unsigned(parser_t p) {
  char as_string[128];
  size_t n;
  for (n = 0; isdigit(peekchar()) && n + 1 < 128;) {
    as_string[n++] = getchar();
  }
  as_string[n] = '\0';
  size_t x;
  sscanf(as_string, "%zu", &x);
  // printf("s:%zu:", x);
  return x;
}

// [^\S\n]{%n,}
void parser_eat_spaces(parser_t p, size_t n) {
  size_t spaces_eaten = 0;
  for (; isspace(peekchar()) && peekchar() != '\n'; getchar()) ++spaces_eaten;
  CHECK_INPUT(1, spaces_eaten < n);
}

// %c
void parser_eat_symbol(parser_t p, char c) {
  CHECK_INPUT(1, peekchar() != c);
  getchar();
}

// \s*$
void parser_eat_trailing_whitespace(parser_t p) {
  while (isspace(peekchar())) getchar();
  CHECK_INPUT(1, peekchar() != EOF);
}

// Skips spaces, reads numbers until an LF, eats the LF.
vector_t parser_read_vector(parser_t p) {
  vector_t v = new_vector();
  for (size_t i = 0; peekchar() != '\n'; ++i) {
    vector_append(v, parser_eat_unsigned(p));
    parser_eat_spaces(p, (peekchar() == '\n' ? 0 : 1));
    // vector_print(v), printf("\n");
  }
  parser_eat_symbol(p, '\n');
  return v;
}

bitset_t parser_read_board(parser_t p, size_t n) {
  bitset_t board = new_bitset(n);

  if (peekchar() == 'R') {
    parser_eat_symbol(p, 'R');

    size_t a = (parser_eat_spaces(p, 0), parser_eat_unsigned(p));
    size_t b = (parser_eat_spaces(p, 1), parser_eat_unsigned(p));
    size_t m = (parser_eat_spaces(p, 1), parser_eat_unsigned(p));
    size_t r = (parser_eat_spaces(p, 1), parser_eat_unsigned(p));
    size_t s_0 = (parser_eat_spaces(p, 1), parser_eat_unsigned(p));

    // printf("%zu %zu %zu %zu %zu\n", a, b, m, r, s_0);

    CHECK_INPUT(4, a > UINT32_MAX);
    CHECK_INPUT(4, b > UINT32_MAX);
    CHECK_INPUT(4, m > UINT32_MAX);
    CHECK_INPUT(4, r > UINT32_MAX);
    CHECK_INPUT(4, s_0 > UINT32_MAX);

    size_t s_i = s_0;
    for (size_t i = 1; i <= r; ++i) {
      s_i = (s_i * a % m + b) % m;
      size_t w_i = s_i % n;

      size_t step = ((size_t)1 << 32);
      for (size_t j = w_i; j < (n < step ? n : n - step + 1);
           j += ((size_t)1 << 32)) {
        // printf("%zu ", j);
        bitset_set(board, j, 1);
      }
    }

  } else {
    parser_eat_symbol(p, '0'), parser_eat_symbol(p, 'x');

    // First we read the number so that the i-th most significant digit is in
    // the i-th bit of the bitset.
    bool is_leading = true;
    size_t n_bin_digits = 0;
    while (hexdigit(peekchar()) >= 0) {
      int hex_digit = hexdigit(peekchar());
      getchar();

      for (size_t i = 4; i-- > 0;) {
        int bin_digit = (((size_t)hex_digit) & (1 << i)) > 0;
        if (bin_digit == 0 && is_leading) continue;
        is_leading = false;
        CHECK_INPUT(4, n_bin_digits + 1 > n);
        bitset_set(board, n_bin_digits++, bin_digit);
      }
    }

    // Then we correct the order of the bits so that the i-th least
    // significant digit is the i-th bit of the bitset.
    if (n_bin_digits < 1) n_bin_digits = 1;
    for (size_t i = 0, j = n_bin_digits - 1; i < j; ++i, --j)
      bitset_swap(board, i, j);
  }

  parser_eat_trailing_whitespace(p);

  return board;
}
