struct line_data {
	
	char *new_label;

	size_t opcode_sz; // 0, 1, 2, or 3 bytes?
	int opcode; // TODO: little endian or big endian?

	size_t operand_sz; // 0, 1, or 2 bytes?
	int operand_is_literal;
	union { // literal and label references are mutually exclusive
		int operand_literal;
		char *operand_label;
	};

};

// returns number of bytes used by the line's instruction
size_t parseline(char *line, struct line_data *data);
