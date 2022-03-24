
#include <stdint.h>
#include <stdlib.h>

size_t mulcap(size_t a, size_t b) {
  size_t x = a * b;
  return a != 0 && x / a != b ? SIZE_MAX : x;
}
