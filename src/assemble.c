#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symtable.h"
#include "parseline.h"
#include "util.h"

#include "assemble.h"

void assemble(FILE *fp, const struct symbol *symtable_head, char *memory) {

	// this can only be different to address when using .PHASE (which may not ever be implemented)
	uintptr_t memindex = 0;

	int line_no = 1;
	int address = 0;
	char line[2048];
	
	while (fgets(line, sizeof(line), fp)) {
		size_t len = strlen(line);
		if (len == 0) continue;
		if (line[len - 1] != '\n') {
			if (fgetc(fp) == EOF) {
				line[len - 1] = '\n';
			} else {
				die("Line too long");
			}
		}
		struct line_data data;
		size_t line_assembled_size = parseline(line, &data, line_no);

		if (data.opcode_sz > 0) {
			// regular instruction

			// copy opcode
			for (unsigned int i = 0; i < data.opcode_sz; i++) {
				memory[memindex++] = data.opcode[i];
			}
			// get operand
			uint16_t operand;
			if (data.operand_label[0] != '\0') {
				int symbol_value = symtable_search(symtable_head, data.operand_label);

#ifdef DEBUG
				printf("ASM: operand_label: %s, value: %d\n", data.operand_label, symbol_value);
#endif

				if (symbol_value == -1) {
					fprintf(stderr, "Undefined label %s on line %d\n", data.operand_label, line_no);
					fprintf(stderr, "halt = %2X opcode[0] = %2X (hex) label[0] = %2X (hex)\n", HALT, data.opcode[0], data.operand_label[0]);
					die("Unable to find symbol");
				}
				operand = (unsigned)symbol_value & 0xFFFF;
			} else {
				operand = data.operand_literal;
			}

			// copy operand
		
			// compute offset if instruction is relative jump
			if (	data.opcode_sz == 1			&&
				   (data.opcode[0] == JR_N		||
					data.opcode[0] == JR_NZ_N	||
					data.opcode[0] == JR_Z_N	||
					data.opcode[0] == JR_NC_N	||
					data.opcode[0] == JR_C_N)	) {
				signed int offset = (signed int)operand - (signed int)address - 2;
				if (offset > 127 || offset < -128) {
					fprintf(stderr, "Cannot perform relative jump to $%4X from line %d\n", operand, line_no);
					fprintf(stderr, "data.opcode = %2X\n", data.opcode[0]);
					die("Relative jump too far");
				}
				operand = (int8_t)offset;
			}
			for (unsigned int i = 0; i < data.operand_sz; i++) {
				memory[memindex++] = (operand >> (i * 8)) & 0xFF;
			}

		} else {
			if (data.pseudo_op == PSEUDO_ORG) {
				address = data.operand_literal;
				memindex = address;
			}
		}
		address += line_assembled_size;
		line_no++;
	}

}
