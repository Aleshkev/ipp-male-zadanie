#include <stdlib.h>

struct _vector_impl;
typedef struct _vector_impl *vector_t;

vector_t new_vector();
vector_t vector_copy(vector_t w);

void vector_append(vector_t this, size_t x);
size_t vector_pop(vector_t this);

size_t vector_size(vector_t this);
size_t vector_get(vector_t this, size_t i);
size_t vector_set(vector_t this, size_t i, size_t x);

void vector_print(vector_t this);
