#include <stdlib.h>

void throw_error(int code, char *info);

void *safe_calloc(size_t n_items, size_t item_size);
void *safe_realloc(void *ptr, size_t n_items, size_t item_size);

#define ALLOC_N(PTR, N) PTR = safe_calloc(N, sizeof *PTR)
#define ALLOC(PTR) PTR = safe_calloc(1, sizeof *PTR)
#define REALLOC_N(PTR, N) PTR = safe_realloc(PTR, N, sizeof *PTR)

void clean_up();
