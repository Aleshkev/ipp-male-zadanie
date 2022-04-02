#include "parser.h"

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "arithmetic.h"
#include "bitset.h"
#include "error.h"
#include "vector.h"

// This is global, because the input stream is global anyway.
int line = 1;

// Peek at the next character in the input stream.
int peekchar() {
  int c = getchar();
  ungetc(c, stdin);
  return c;
}

// Check if an ASCII character is a line separator in Unicode.
bool isnewline(int c) { return '\n' <= c && c <= '\r'; }

// Return the int value of a character interpreted as a hexadecimal digit, or -1
// if the character is not a hexadecimal digit.
int hexdigit(int c) {
  c = tolower(c);
  return '0' <= c && c <= '9'   ? c - '0'  //
         : 'a' <= c && c <= 'f' ? 10 + c - 'a'
                                : -1;
}

// Eat a single non-negative number.
size_t parser_eat_unsigned(int line) {
  bool overflow = false;
  size_t x = 0, n_digits = 0;
  for (; isdigit(peekchar()); ++n_digits)
    x = addcap(mulcap(x, 10, &overflow), getchar() - '0', &overflow);
  CHECK_INPUT(line, overflow);
  CHECK_INPUT(line, n_digits == 0);
  return x;
}

void parser_eat_spaces(int line, size_t at_least_so_many) {
  size_t spaces_eaten = 0;
  for (; isspace(peekchar()) && !isnewline(peekchar()); getchar())
    ++spaces_eaten;
  CHECK_INPUT(line, spaces_eaten < at_least_so_many);
}

void parser_eat_symbol(int line, char symbol) {
  CHECK_INPUT(line, peekchar() != symbol);
  getchar();
}

// Eat a newline character. The only function that should be eating any.
void parser_eat_newline(int line) {
  CHECK_INPUT(line, !isnewline(peekchar()));
  getchar();
}

void parser_eat_eof(int line) {
  if (peekchar() != EOF) parser_eat_newline(line);
  parser_eat_symbol(line, EOF);
}

void parser_eat_trailing_whitespace(int line) {
  while (isspace(peekchar())) getchar();
  CHECK_INPUT(line, peekchar() != EOF);
}

// Read numbers until the end of the line.
vector_t parser_eat_vector(int line) {
  vector_t v = new_vector();
  parser_eat_spaces(line, 0);
  for (size_t i = 0; !isnewline(peekchar()) && peekchar() != EOF; ++i) {
    vector_append(v, parser_eat_unsigned(line));
    parser_eat_spaces(line, 0);
    // vector_print(v), printf("\n");
  }
  return v;
}

bitset_t parser_eat_board(int line, size_t n) {
  bitset_t board = new_bitset(n);

  parser_eat_spaces(line, 0);

  if (peekchar() == 'R') {
    parser_eat_symbol(line, 'R');

    vector_t nums = new_vector();
    for (size_t i = 0; i < 5; ++i) {
      parser_eat_spaces(line, (i == 0 ? 0 : 1));
      vector_append(nums, parser_eat_unsigned(line));
      CHECK_INPUT(4, vector_get(nums, i) > UINT32_MAX);
    }
    size_t a = vector_get(nums, 0), b = vector_get(nums, 1),
           m = vector_get(nums, 2), r = vector_get(nums, 3),
           s_0 = vector_get(nums, 4);
    // printf("%zu %zu %zu %zu %zu\n", a, b, m, r, s_0);

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
    parser_eat_symbol(line, '0'), parser_eat_symbol(line, 'x');

    // First we read the number so that the i-th most significant digit is in
    // the i-th bit of the bitset.
    bool is_leading = true;
    bool empty_number = true;
    size_t n_bin_digits = 0;
    while (hexdigit(peekchar()) >= 0) {
      empty_number = false;
      int hex_digit = hexdigit(peekchar());
      getchar();

      // printf(",");
      for (size_t i = 4; i-- > 0;) {
        int bin_digit = (((size_t)hex_digit) & (1 << i)) > 0;
        if (bin_digit == 0 && is_leading) continue;
        is_leading = false;
        CHECK_INPUT(4, n_bin_digits + 1 > n);
        // if(n_bin_digits % 1000 == 0) printf("%zu %zu\n", n_bin_digits, n);
        bitset_set(board, n_bin_digits++, bin_digit);
      }
    }
    CHECK_INPUT(line, empty_number);

    // Then we correct the order of the bits so that the i-th least
    // significant digit is the i-th bit of the bitset.
    if (n_bin_digits < 1) n_bin_digits = 1;
    for (size_t i = 0, j = n_bin_digits - 1; i < j; ++i, --j)
      bitset_swap(board, i, j);
  }
  parser_eat_spaces(line, 0);

  return board;
}
