#include <stdio.h>
#include <string.h>

#include "symtable.h"
#include "pass2.h"

void
pass2_assemble(FILE* fp, char *mem, size_t mem_sz,
	       symtable_entry_t *tab, size_t tab_sz)
{
	char some_string[64];
	sprintf(some_string, "%d", tab[0].value);
	strcpy(mem, some_string);
}
