#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "instructions.h"
#include "parseline.h"

// returns number of bytes used by the line's instruction
size_t parseline(char *line, struct line_data *data)
{
	if (line[0] != '\n') {
		int size = strlen(line); // size doesn't including \0
		if (line[size - 1] == '\n') {
			line[size - 1] = 0;
			size -= 1;
		}
		data->new_label = malloc(size + 1);
		strcpy(data->new_label, line);
	} else {
		data->new_label = NULL;
	}
	data->is_pseudo_op = 0;
	data->opcode_sz = 2;
	data->opcode = 0xED4B; // ld bc, (**)
	data->operand_sz = 2;
	data->operand_is_literal = 1;
	data->operand_literal = 0xBEEF;
	return data->opcode_sz + data->operand_sz;
}
