#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symtable.h"

symtable_entry_t *
symtable_build(FILE* fp, size_t sz)
{
	symtable_entry_t *tab = malloc(sizeof(symtable_entry_t) * sz);
	char line[256];

	// loop until it has reached EOF or the end of the symtable
	for (int i = 0; (i < sz) && fgets(line, sizeof(line), fp); i++) {
		tab[i].name = malloc(sizeof(line));
		strcpy(tab[i].name, line);
	}

	return tab;
}

void
symtable_cleanup(symtable_entry_t *tab, size_t sz)
{
	for (int i = 0; i < sz; i++)
		free(tab[i].name);	// free() does nothing if 'name' is null
	free(tab);
}
