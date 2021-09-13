struct symbol {
	char *label;
	int value;
	struct symbol *next;
};

size_t symtable_len(struct symbol *symtable_head);
size_t symtable_build(FILE *fp, struct symbol **symtable_head);
