#include "bitset.h"
#include "error.h"
#include "grid.h"

grid_t parser_eat_grid();
size_t parser_eat_position(grid_t grid);
bitset_t parser_eat_board(size_t n);
void parser_eat_newline();
void parser_eat_eof();

void parser_throw_error(char *info);
#define parser_error_if(CONDITION) \
  (CONDITION ? parser_throw_error(#CONDITION), 0 : 0)
void parser_throw_error_retro(int line, char *info);
#define parser_error_retro_if(LINE, CONDITION) \
  (CONDITION ? parser_throw_error_retro(LINE, #CONDITION), 0 : 0)
