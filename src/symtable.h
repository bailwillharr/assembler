struct symbol {
	char *label;
	int value;
	struct symbol *next;
};

size_t symtable_build(FILE *fp, struct symbol *symtable_head);
