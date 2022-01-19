#ifndef PARSELINE_H
#define PARSELINE_H

#include <inttypes.h>

#define LABEL_MAX_LEN 15
#define OPCODE_NAME_MAX_LEN 4

enum OpcodesPseudo { PSEUDO_DB, PSEUDO_DW, PSEUDO_ORG };

struct line_data {
	
	char *new_label; // empty if no label is defined

	ssize_t opcode_sz; // if 0, no opcode
	uint8_t opcode[3]; // start at [0]
	enum OpcodesPseudo pseudo_op; // use if opcode_sz == -1

	size_t operand_sz; // if 0, no operand
	char operand_label[LABEL_MAX_LEN+1]; // when empty, use operand_literal instead
	uint16_t operand_literal; // little endian
};

// returns number of bytes used by the line's instruction
size_t parseline(char *line, struct line_data *data, int line_no);

#endif
