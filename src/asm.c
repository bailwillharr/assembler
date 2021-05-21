#include <stdio.h>
#include <stdlib.h>

#include "asm.h"
#include "util.h"
#include "symtable.h"
#include "pass2.h"

#define SYMTABLE_SIZE 16

int
asm_assemble(FILE *fp, char *mem, size_t sz)
{

	symtable_entry_t *tab;
	tab = symtable_build(fp, SYMTABLE_SIZE);

	for (int i = 0; tab[i].name != NULL && i < SYMTABLE_SIZE; i++) {
		printf("tab[%d].name = %s\n", i, tab[i].name);
		printf("tab[%d].value = %d\n", i, tab[i].value);
	}

	// rewind the file ptr
	fseek(fp, 0, SEEK_SET);

	pass2_assemble(fp, mem, sz, tab, SYMTABLE_SIZE);

	symtable_cleanup(tab, SYMTABLE_SIZE);

	return EXIT_SUCCESS;

}
