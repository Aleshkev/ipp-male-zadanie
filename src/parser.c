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
#include "grid.h"
#include "vector.h"

// This module implements an elegant parser that eats the input, character by
// character, raising appropriate errors. It has a global state, because the
// input stream is global anyway. Other modules may use parser_throw_error() to
// indicate that the data passed to them, coming from the input, is invalid
// (especially grid.c).

// The line of the input we're reading right now.
int current_line = 1;

// Notify the parser there's an input error in the current line.
void parser_throw_error(char *info) { throw_error(current_line, info); }

// Notify the parser there has had been an input error but some previous
// line should be reported as the source.
void parser_throw_error_retro(int line, char *info) { throw_error(line, info); }

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

// Consume a single size_t integer.
size_t parser_eat_unsigned() {
  bool overflow = false;
  size_t x = 0, n_digits = 0;
  for (; isdigit(peekchar()); ++n_digits)
    x = addcap(mulcap(x, 10, &overflow), getchar() - '0', &overflow);
  parser_error_if(overflow);
  parser_error_if(n_digits == 0);
  return x;
}

// Tuck away spaces, with a minimum amount.
void parser_eat_spaces(size_t at_least_so_many) {
  size_t spaces_eaten = 0;
  for (; isspace(peekchar()) && !isnewline(peekchar()); getchar())
    ++spaces_eaten;
  parser_error_if(spaces_eaten < at_least_so_many);
}

// Swallow a single character, assert it's the symbol.
void parser_eat_symbol(char symbol) {
  parser_error_if(peekchar() != symbol);
  getchar();
}

// Devour a newline character. The only function that should be eating any, as
// it needs to update the global line counter.
//
// If there's an EOF instead of a newline, this will raise an error with the
// the current line as the 'first line relating to the error'. Some tests argue
// the errror should be considered to relate only to the next, nonexistent line.
// I disagree.
void parser_eat_newline() {
  parser_error_if(!isnewline(peekchar()));
  getchar();
  ++current_line;
}

// Munch on an end-of-file. Because apparently no newline at the end of the
// input is valid, it handles eating the last newline in the input as well.
void parser_eat_eof() {
  if (peekchar() != EOF) parser_eat_newline();
  parser_eat_symbol(EOF);
}

// Ingest numbers until the end of the line.
vector_t parser_eat_vector() {
  vector_t v = new_vector();
  parser_eat_spaces(0);
  for (size_t i = 0; !isnewline(peekchar()) && peekchar() != EOF; ++i) {
    vector_append(v, parser_eat_unsigned());
    parser_eat_spaces(0);
  }
  return v;
}

// Eat information about the grid. Up to the end of the line.
grid_t parser_eat_grid() { return new_grid(parser_eat_vector()); }

// Consume information about a position on the grid. Up to the end of the line.
size_t parser_eat_position(grid_t grid) {
  return grid_rank(grid, parser_eat_vector());
}

// Feast on a bitset encoded using this funny form from the task specification.
// Up to the end of the line.
bitset_t parser_eat_board(size_t n) {
  bitset_t board = new_bitset(n);

  // This is a direct, sequential, easy-to-understand implementation of the
  // specification, so it's allowed to be two pages long.

  parser_eat_spaces(0);
  if (peekchar() == 'R') {
    parser_eat_symbol('R');

    vector_t nums = new_vector();
    for (size_t i = 0; i < 5; ++i) {
      parser_eat_spaces(i == 0 ? 0 : 1);
      vector_append(nums, parser_eat_unsigned());
      parser_error_if(vector_get(nums, i) > UINT32_MAX);
    }
    size_t a = vector_get(nums, 0), b = vector_get(nums, 1),
           m = vector_get(nums, 2), r = vector_get(nums, 3),
           s_0 = vector_get(nums, 4);
    parser_error_if(m == 0);

    size_t s_i = s_0;
    for (size_t i = 1; i <= r; ++i) {
      s_i = (s_i * a % m + b) % m;
      size_t w_i = s_i % n;

      size_t step = ((size_t)1 << 32);
      for (size_t j = w_i; j < (n < step ? n : n - step + 1);
           j += ((size_t)1 << 32)) {
        bitset_set(board, j, 1);
      }
    }
  } else {
    parser_eat_symbol('0'), parser_eat_symbol('x');

    // First we read the number so that the i-th most significant digit is in
    // the i-th bit of the bitset.
    bool is_leading = true;
    bool empty_number = true;
    size_t n_bin_digits = 0;
    while (hexdigit(peekchar()) >= 0) {
      empty_number = false;
      int hex_digit = hexdigit(peekchar());
      getchar();

      for (size_t i = 4; i-- > 0;) {
        int bin_digit = (((size_t)hex_digit) & (1 << i)) > 0;
        if (bin_digit == 0 && is_leading) continue;
        is_leading = false;
        parser_error_if(n_bin_digits + 1 > n);
        bitset_set(board, n_bin_digits++, bin_digit);
      }
    }
    parser_error_if(empty_number);

    // Then we correct the order of the bits so that the i-th least
    // significant digit is the i-th bit of the bitset.
    if (n_bin_digits < 1) n_bin_digits = 1;
    for (size_t i = 0, j = n_bin_digits - 1; i < j; ++i, --j)
      bitset_swap(board, i, j);
  }
  parser_eat_spaces(0);

  return board;
}
