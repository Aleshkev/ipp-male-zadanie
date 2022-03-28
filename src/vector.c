#include "vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"

typedef size_t T;

struct _vector_impl {
  size_t n_elements, space;
  T *elements;
};
typedef struct _vector_impl *vector_t;

// Creates a new empty vector. Returns the object, or NULL on alloc failure.
vector_t new_vector() {
  vector_t v;
  ALLOC(v);

  v->n_elements = 0, v->space = 8;
  ALLOC_N(v->elements, v->space);

  return v;
}

void vector_reserve(vector_t this, size_t n) {
  while (n + 1 >= this->space) {
    this->space *= 2;
    REALLOC_N(this->elements, this->space);
  }
}

// Appends an element. Returns the object, or NULL on alloc failure.
void vector_append(vector_t this, size_t x) {
  vector_reserve(this, this->n_elements + 1);
  this->elements[this->n_elements++] = x;
}

vector_t vector_copy(vector_t w) {
  vector_t v = new_vector();
  for (size_t i = 0; i < vector_size(w); ++i) {
    vector_append(v, vector_get(w, i));
  }
  return v;
}

// Removes an element from the end of the vector. Never fails.
void vector_pop(vector_t v) {
  if (v == NULL) return;

  assert(v->n_elements > 0);

  v->elements[v->n_elements--] = 0;
}

// These never fail.

size_t vector_size(vector_t v) { return v->n_elements; }
T vector_get(vector_t v, size_t i) { return v->elements[i]; }
T vector_set(vector_t v, size_t i, T x) { return v->elements[i] = x; }
T *vector_ref(vector_t v, size_t i) { return &v->elements[i]; }

void vector_print(vector_t v) {
  printf("[");
  for (size_t i = 0; i < v->n_elements; ++i) {
    if (i > 0) printf(", ");
    printf("%zu", v->elements[i]);
  }
  printf("]");
}
