
typedef size_t T;
typedef struct _vector_impl *vector_t;

vector_t vector_calloc();
void vector_free(vector_t v);
vector_t *vector_append(vector_t v, size_t x);
void vector_pop(vector_t v);
size_t vector_size(vector_t v) { return v->n_elements; }
T vector_get(vector_t v, size_t i) { return v->elements[i]; }
T vector_set(vector_t v, size_t i, T x) { return v->elements[i] = x; }
T *vector_ref(vector_t v, size_t i) { return &v->elements[i]; }
