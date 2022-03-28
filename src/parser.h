
#include "bitset.h"
#include "vector.h"

struct _parser_impl;
typedef struct _parser_impl *parser_t;

parser_t new_parser();
vector_t parser_read_vector(parser_t p);
bitset_t parser_read_bitset(parser_t p, size_t n);
void parser_eat_trailing_whitespace(parser_t p);

bool parser_is_input_ok(parser_t p);
bool parser_is_system_ok(parser_t p);