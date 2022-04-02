#include "vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"

struct _vector_impl {
  size_t n_elements, space;
  size_t *elements;
};
typedef struct _vector_impl *vector_t;

// Create an empty vector. A vector stores integers, and resizes itself if
// necessary.
vector_t new_vector() {
  vector_t v;
  ALLOC(v);

  v->n_elements = 0, v->space = 8;
  ALLOC_N(v->elements, v->space);

  return v;
}

// Create a copy of another vector.
vector_t vector_copy(vector_t w) {
  vector_t v = new_vector();
  for (size_t i = 0; i < vector_size(w); ++i)
    vector_append(v, vector_get(w, i));
  return v;
}

void vector_reserve(vector_t this, size_t n) {
  while (n + 1 >= this->space)
    this->space *= 2, REALLOC_N(this->elements, this->space);
}

// Append (push back) an element.
void vector_append(vector_t this, size_t x) {
  vector_reserve(this, this->n_elements + 1);
  this->elements[this->n_elements++] = x;
}

// Remove an element from the end of the vector and return its value.
size_t vector_pop(vector_t v) {
  assert(v->n_elements > 0);
  size_t x = v->elements[v->n_elements - 1];
  v->elements[v->n_elements--] = 0;
  return x;
}

// Number of elements the vector holds.
size_t vector_size(vector_t v) { return v->n_elements; }

size_t *vector_ref(vector_t v, size_t i) {
  assert(i < v->n_elements);
  return &v->elements[i];
}
size_t vector_get(vector_t v, size_t i) { return *vector_ref(v, i); }
size_t vector_set(vector_t v, size_t i, size_t x) {
  return *vector_ref(v, i) = x;
}

void vector_print(vector_t v) {
  printf("(vector_t){");
  for (size_t i = 0; i < v->n_elements; ++i) {
    if (i > 0) printf(", ");
    printf("%zu", v->elements[i]);
  }
  printf("}");
}
