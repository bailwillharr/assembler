struct line_data {
	
	char *new_label;

	int is_pseudo_op;

	size_t opcode_sz; // 0, 1, 2, or 3 bytes?
	int opcode;

	size_t operand_sz; // 0, 1, or 2 bytes?
	int operand_is_literal;
	// These two variables are mutually exclusive:
	int operand_literal;
	char *operand_label;

};

// returns number of bytes used by the line's instruction
size_t parseline(char *line, struct line_data *data);
