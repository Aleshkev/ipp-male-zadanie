#include <assert.h>
#include <stdlib.h>

#include "vector.h"

typedef size_t T;

struct _vector_impl {
  size_t n_elements, space;
  T *elements;
};
typedef struct _vector_impl *vector_t;

// Creates a new empty vector. Returns the object, or NULL on alloc failure.
vector_t vector_calloc() {
  vector_t v = malloc(sizeof *v);
  if (v == NULL) return NULL;

  v->n_elements = 0, v->space = 8;

  v->elements = calloc(v->space, sizeof *v->elements);
  if (v->elements == NULL) return free(v), NULL;

  return v;
}

// Frees. Nothing to do if it fails.
void vector_free(vector_t v) {
  free(v->elements), v->elements = NULL;
  free(v);
}

// Appends an element. Returns the object, or NULL on alloc failure.
vector_t *vector_append(vector_t v, size_t x) {
  while (v->n_elements + 1 < v->space) {
    v->space *= 2;

    v->elements = realloc(v->elements, v->space * sizeof *v->elements);
    if (v->elements == NULL) return free(v), NULL;
  }
  v->elements[v->n_elements++] = x;
  return v;
}

// Removes an element from the end of the vector. Never fails.
void vector_pop(vector_t v) {
  assert(v->n_elements > 0);

  v->elements[v->n_elements--] = 0;
}

// These never fail.

size_t vector_size(vector_t v) { return v->n_elements; }
T vector_get(vector_t v, size_t i) { return v->elements[i]; }
T vector_set(vector_t v, size_t i, T x) { return v->elements[i] = x; }
T *vector_ref(vector_t v, size_t i) { return &v->elements[i]; }
