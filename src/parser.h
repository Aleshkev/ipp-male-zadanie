#include "bitset.h"
#include "vector.h"

vector_t parser_eat_vector(int line);
bitset_t parser_eat_board(int line, size_t n);
void parser_eat_newline(int line);
void parser_eat_eof(int line);
