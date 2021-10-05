#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdio.h>

struct symbol {
	char *label;
	int value;
	struct symbol *next;
};

size_t symtable_len(const struct symbol *symtable_head);
int symtable_search(const struct symbol *symtable_head, const char *label);
size_t symtable_build(FILE *fp, struct symbol **symtable_head);

#endif
