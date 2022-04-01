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

  p->system_ok = p->input_ok = true;
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
         : 'a' <= c && c <= 'f' ? c - 'a'
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

#define INPUT_ERROR()    \
  {                      \
    p->input_ok = false; \
    return;              \
  }

// [^\S\n]{%n,}
void parser_eat_spaces(parser_t p, size_t n) {
  size_t eaten = 0;
  for (; isspace(peekchar()) && peekchar() != '\n'; getchar()) ++eaten;
  if (eaten < n) INPUT_ERROR();
}

// %c
void parser_eat_symbol(parser_t p, char c) {
  if (peekchar() != c) INPUT_ERROR();
  getchar();
}

// \s*$
void parser_eat_trailing_whitespace(parser_t p) {
  while (isspace(peekchar())) getchar();
  if (peekchar() != EOF) INPUT_ERROR();
}

// Skips spaces, reads numbers until an LF, eats the LF.
vector_t parser_read_vector(parser_t p) {
  vector_t v = new_vector();

  for (size_t i = 0; peekchar() != '\n' && p->input_ok; ++i) {
    parser_eat_spaces(p, (i == 0 ? 0 : 1));

    size_t x = parser_eat_unsigned(p);
    vector_append(v, x);
    // vector_print(v), printf("\n");
  }
  parser_eat_symbol(p, '\n');
  return v;
}

bitset_t parser_read_bitset(parser_t p, size_t n) {
  bitset_t b = new_bitset(n);

  if (peekchar() == 'R') {
    parser_eat_symbol(p, 'R');

    size_t a = (parser_eat_spaces(p, 0), parser_eat_unsigned(p));
    size_t b = (parser_eat_spaces(p, 1), parser_eat_unsigned(p));
    size_t m = (parser_eat_spaces(p, 1), parser_eat_unsigned(p));
    size_t r = (parser_eat_spaces(p, 1), parser_eat_unsigned(p));
    size_t s0 = (parser_eat_spaces(p, 1), parser_eat_unsigned(p));

  } else {
    parser_eat_symbol(p, '0'), parser_eat_symbol(p, 'x');
    bool is_leading = true;
    size_t n_bin_digits = 0;
    while (hexdigit(peekchar()) >= 0) {
      int hex_digit = hexdigit(peekchar());
      getchar();

      for (size_t i = 4; i-- > 0;) {
        int bin_digit = (((size_t)hex_digit) & (1 << i));
        if (bin_digit == 0 && is_leading) continue;
        is_leading = false;
        ++n_bin_digits;
        if (n_bin_digits >= n) {
          p->input_ok = false;
          return b;
        }
        bitset_set(b, n - n_bin_digits++ - 1, bin_digit);
      }
    }
    size_t shift_by = n - n_bin_digits;
    for (size_t i = 0; i < n_bin_digits; ++i)
      bitset_set(b, i, bitset_get(b, i + shift_by));
    for (size_t i = n_bin_digits; i < n; ++i) bitset_set(b, i, 0);
  }

  parser_eat_trailing_whitespace(p);

  return b;
}

bool parser_is_input_ok(parser_t p) { return p->input_ok; }
bool parser_is_system_ok(parser_t p) { return p->system_ok; }
