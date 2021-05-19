#include <stdio.h>
#include <stdlib.h>

#include "symtable.h"

symtable_entry_t*
symtable_build(FILE* fp, size_t sz)
{
	symtable_entry_t* tab = malloc(sizeof(symtable_entry_t) * sz);
	for (int i = 0; i < sz; i++) {
		tab[i].name = "hello";
	}
	return tab;
}

void
symtable_cleanup(symtable_entry_t* tab)
{
	free(tab);
}
