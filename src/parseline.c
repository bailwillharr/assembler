#include <stdlib.h>

#include "parseline.h"

// returns number of bytes used by the line's instruction
size_t parseline(char *line, struct line_data *data)
{
	data->new_label = NULL;
	data->opcode_sz = 2;
	data->opcode = 0xED4B; // ld bc, ($BEEF), random choice
	data->operand_sz = 2;
	data->operand_is_literal = 1;
	data->operand_literal = 0xBEEF;
	return data->opcode_sz + data->operand_sz;
}
