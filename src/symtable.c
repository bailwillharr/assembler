#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "util.h"
#include "symtable.h"
#include "instructions.h"
#include "parseline.h"

/*
 * This symtable will read backwards to the order items are added.
 */

size_t symtable_len(struct symbol *symtable_head)
{
	size_t len = 0;
	struct symbol *current;
	for (current = symtable_head; current != NULL; current = current->next) {
		len += 1;
	}
	return len;
}

/* 
 * Loop through 'fp' and keep track of the current address.
 * This will require reading opcodes and operands but not
 * adding them to the output file.
 */
size_t symtable_build(FILE *fp, struct symbol **symtable_head)
{
	int address = 0;
	char line[2048];
	while (fgets(line, sizeof(line), fp)) {
		size_t len = strlen(line);
		if (len == 0) continue;
		if (line[len - 1] != '\n') {
			die("Line too long");
		}
		struct line_data data;
		size_t line_assembled_size = parseline(line, &data);

		// debug
		printf("Opcode: %4X, Operand: %4X, Address: %d\n", data.opcode, data.operand_literal, address);

		if (data.new_label != NULL) {
			struct symbol *new_symbol = calloc(sizeof(struct symbol), 1);
			new_symbol->label = data.new_label;
			new_symbol->value = address;
			new_symbol->next = *symtable_head;
			*symtable_head = new_symbol;
		}
		address += line_assembled_size;
	}
	
	return (size_t)address;
}
