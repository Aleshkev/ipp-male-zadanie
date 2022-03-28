#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"

void memory_error() {
  printf("ERROR 0\n");
  exit(1);
}

void input_error(int code, char *info) {
  fprintf(stdout, "\033[0;91m%s\033[0m\n", info);
  fprintf(stderr, "ERROR %i\n", code);

  clean_up(), exit(-1);
}

void no_error_error() { clean_up(), exit(0); }

#define MAX_N_ALLOCATED 1024
void *allocated[MAX_N_ALLOCATED], *unallocated[MAX_N_ALLOCATED];
size_t n_allocated = 0, n_unallocated = 0;

void *safe_calloc(size_t n_items, size_t item_size) {
  void *ptr = calloc(n_items, item_size);
  if (ptr == NULL) memory_error();
  allocated[n_allocated++] = ptr;
  return ptr;
}
void *safe_realloc(void *ptr, size_t n_items, size_t item_size) {
  assert(ptr != NULL);
  unallocated[n_unallocated++] = ptr;
  void *new_ptr = realloc(ptr, n_items * item_size);
  if (new_ptr == NULL) memory_error();
  allocated[n_allocated++] = new_ptr;
  return new_ptr;
}

void clean_up() {
  // TODO
}
