#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symtable.h"
#include "parseline.h"
#include "util.h"

#include "assemble.h"

const uint8_t JR_N		= 0x18;
const uint8_t JR_NZ_N	= 0x20;
const uint8_t JR_Z_N	= 0x28;
const uint8_t JR_NC_N	= 0x30;
const uint8_t JR_C_N	= 0x38;
const uint8_t DJNZ_N	= 0x10;

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

#ifndef NDEBUG
				fprintf(stderr, "ASM: operand_label: %s, value: %d\n", data.operand_label, symbol_value);
#endif

				if (symbol_value == -1) {
					die("undefined label %s on line %d\n", data.operand_label, line_no);
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
					data.opcode[0] == JR_C_N	||
					data.opcode[0] == DJNZ_N)	) {
				signed int offset = (signed int)operand - (signed int)address - 2;
				if (offset > 127 || offset < -128) {
					fprintf(stderr, "Cannot perform relative jump to $%4X (line %d)\n", operand, line_no);
					die("relative jump too far");
				}
				operand = (int8_t)offset;
			}
			for (unsigned int i = 0; i < data.operand_sz; i++) {
				memory[memindex++] = (operand >> (i * 8)) & 0xFF;
			}

		} else if (data.opcode_sz == -1) {
			if (data.pseudo_op == PSEUDO_ORG) {
				address = data.operand_literal;
				memindex = address;
			}
		}
		address += line_assembled_size;
		line_no++;
	}

}
