#include <stdio.h>
#include <stdlib.h>

#include "symtable.h"

int symtable_build(FILE* fp, symtable_entry_t** table, size_t sz)
{
	for (int i = 0; i < sz; i++) {
		(*table)[i].name = "Test";
	}
	return EXIT_SUCCESS;
}
