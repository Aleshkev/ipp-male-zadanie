
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitset.h"
#include "vector.h"

struct _parser_impl {
  bool system_ok, input_ok;
};
typedef struct _parser_impl *parser_t;

parser_t parser_calloc() {
  parser_t p;
  if (p = calloc(1, sizeof *p) == NULL) return NULL;

  p->system_ok = p->input_ok = true;
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

void reversec(char *start, char *stop) {
  for (size_t i = 0; start + 2 * i < stop; ++i) {
    swap(start + i, stop - i - 1);
  }
}

// [0-9]+
size_t parser_eat_unsigned(parser_t p) {
  char as_string[128];
  size_t n;
  for (n = 0; isdigit(peekchar()) && n < 128; ++n) {
    as_string[n++] = getchar();
  }
  reverse(as_string, as_string + n);
}

// [^\S\n]{%n,}
void parser_eat_spaces(parser_t p, size_t n) {
  size_t eaten = 0;
  for (; isspace(peekchar()) && peekchar() != '\n'; getchar()) ++eaten;
  if (eaten < n) return p->input_ok = false;
}

// %c
void parser_eat_symbol(parser_t p, char c) {
  if (peekchar() != c) return p->input_ok = false;
  getchar();
}

// \s*$
void parser_eat_trailing_whitespace(parser_t p) {
  while (isspace(peekchar())) getchar();
  if (peekchar() != EOF) p->input_ok = false;
}

// Skips spaces, reads numbers until an LF, eats the LF.
vector_t parser_read_vector(parser_t p) {
  vector_t v = vector_calloc();
  if (v == NULL) return NULL;

  for (size_t i = 0; isdigit(peekchar()) && p->input_ok; ++i) {
    parser_eat_spaces(p, (i == 0 ? 0 : 1));

    size_t x = parser_eat_unsigned(p);
    v = vector_append(v, x);
    if (v == NULL) return NULL;
  }
  parser_eat_symbol(p, '\n');
  return v;
}

vector_t parser_read_bitset(parser_t p, size_t n) {
  bitset_t b = bitset_calloc(n);
  if (b == NULL) return NULL;

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
    size_t bin_digit_i = 0;
    while (hexdigit(peekchar()) >= 0) {
      int hex_digit = hexdigit(peekchar());
      getchar();

      for (size_t i = 4; i-- > 0;) {
        int bin_digit = (((size_t)hex_digit) & (1 << i));
        if (bin_digit) is_leading = false;
        if (is_leading) continue;

        bitset_set(b, n - bin_digit_i++ - 1, bin_digit);
      }
    }
    size_t n_bin_digits = bin_digit_i;
    size_t shift_by = n - n_bin_digits;
    for (size_t i = 0; i < n_bin_digits; ++i)
      bitset_set(b, i, bitset_get(b, i + shift_by));
    for (size_t i = n_bin_digits; i < n; ++i) bitset_set(b, i, 0);
  }

  parser_eat_trailing_whitespace(p);
}
