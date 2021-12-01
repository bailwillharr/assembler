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

size_t symtable_len(const struct symbol *symtable_head)
{
	size_t len = 0;
	for (const struct symbol *current = symtable_head; current != NULL; current = current->next) {
		len += 1;
	}
	return len;
}

int symtable_search(const struct symbol *head, const char *label) {

	for (const struct symbol *entry = head; entry != NULL; entry = entry->next) {
		if (strcmp(label, entry->label) == 0) {
			return entry->value;
		}
	}

	return -1;

}

/* 
 * Loop through 'fp' and keep track of the current address.
 * This will require reading opcodes and operands but not
 * adding them to the output file.
 */
size_t symtable_build(FILE *fp, struct symbol **symtable_head)
{
	int line_no = 1;
	int address = 0;
	char line[2048];

	while (fgets(line, sizeof(line), fp)) {
		size_t len = strlen(line);
		if (len == 0) continue;
		if (line[len - 1] != '\n') {
			if (fgetc(fp) == EOF) {
				line[len - 1] = '\n';
				// DEBUG
				printf("At end of file\n");
			} else {
				die("Line too long");
			}
		}
		struct line_data data;
		size_t line_assembled_size = parseline(line, &data, line_no);
		free(data.operand_label);

		if (data.new_label != NULL) {
			struct symbol *new_symbol = calloc(sizeof(struct symbol), 1);
			new_symbol->label = data.new_label;
			new_symbol->value = address;
			new_symbol->next = *symtable_head;
			*symtable_head = new_symbol;
		}
		if (data.opcode_sz == -1 && data.pseudo_op == PSEUDO_ORG) {
			if (data.new_label != NULL) die("Cannot define label on same line as .ORG");
			if (data.operand_label != NULL) {
				fprintf(stderr, "ERROR on line %d: .ORG cannot use labels\n", line_no);
				exit(EXIT_FAILURE);
			}
			printf("ADDRESS change to %4X\n", data.operand_literal);
			address = data.operand_literal;
		}
		address += line_assembled_size;
		line_no++;
	}

	return (size_t)address;

}

void symtable_print(struct symbol *symtable_head) {

	puts("\n********SYMBOL TABLE********\n");
	printf("LEN: %lu\n", symtable_len(symtable_head));
	for (struct symbol *current = symtable_head; current != NULL; current = current->next) {
		printf("LABEL: %s\t\tVALUE: %d\t\tPTR: %p\t\tNEXT: %p\n", current->label, current->value, current, current->next);
	}
	puts("\n");

}
