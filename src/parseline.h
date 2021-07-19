struct line_data {
	
	char *new_label; // empty if no label is defined

	int is_pseudo_op;

	// is_pseudo_op == 1
	enum pseudo_opcode opcode_pseudo;

	// is_pseudo_op == 0
	size_t opcode_sz; // 0, 1, 2, or 3 bytes?
	int opcode;



	size_t operand_sz; // 0, 1, or 2 bytes?
	int operand_is_literal;
	// operand_is_literal == 1
	int operand_literal;

	// operand_is_literal == 0
	char *operand_label;

};

// returns number of bytes used by the line's instruction
size_t parseline(char *line, struct line_data *data);
