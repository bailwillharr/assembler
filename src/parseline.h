struct line_data {
	
	char *new_label; // empty if no label is defined

	enum pseudo_opcode pseudo_op; // ignore if 0

	// ignored when pseudo op
	size_t opcode_sz;
	int opcode; // stored little-endian

	size_t operand_sz;
	char *operand_label; // when 0, use operand_literal instead
	int operand_literal; // stored little-endian
};

// returns number of bytes used by the line's instruction
size_t parseline(char *line, struct line_data *data, int line_no);
