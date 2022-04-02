#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// Multiply two non-negative numbers. Return SIZE_MAX and set the flag on
// overflow.
static inline size_t mulcap(size_t a, size_t b, bool *overflow) {
  size_t x = a * b;
  if (a != 0 && x / a != b) return *overflow = true, SIZE_MAX;
  return x;
}

// Add two non-negative numbers. Return SIZE_MAX and set the flag on overflow.
static inline size_t addcap(size_t a, size_t b, bool *overflow) {
  if (b > SIZE_MAX - a) return *overflow = true, SIZE_MAX;
  return a + b;
}