#include "error.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void memory_error() { input_error(0, "<memory error>"); }

void input_error(int code, char *info) {
  // Important.
  if (ferror(stdin)) code = 0, info = "<input stream error>";

#if 1
  fprintf(stdout, "<\033[0;91m%s ∴ %i\033[0m>", info, code);
#endif
  fprintf(stderr, "ERROR %i\n", code);

  clean_up(), exit(-1);
}

// To make sure we don't leak memory, we keep references to all allocated
// memory (and not released by safe_realloc) and explicitly free all of it at
// some point. The number of objects in this project is bounded by a
// source-code-dependent constant, so a limited buffer here doesn't create any
// restrictions on input sizes.

#define MAX_N_ALLOCATED 2137
void *allocated[MAX_N_ALLOCATED];
size_t n_allocated = 0;

void *safe_calloc(size_t n_items, size_t item_size) {
  void *ptr = calloc(n_items, item_size);
  if (ptr == NULL) memory_error();
  allocated[n_allocated++] = ptr;
  return ptr;
}
void *safe_realloc(void *ptr, size_t n_items, size_t item_size) {
  assert(ptr != NULL);

  for (size_t i = 0; i < n_allocated; ++i)
    if (allocated[i] == ptr) {
      allocated[i] = allocated[n_allocated-- - 1];
      break;
    }

  void *new_ptr = realloc(ptr, n_items * item_size);
  if (new_ptr == NULL) memory_error();
  allocated[n_allocated++] = new_ptr;
  return new_ptr;
}

void clean_up() {
  for (size_t i = 0; i < n_allocated; ++i) free(allocated[i]);
  n_allocated = 0;
}
