#include <stdio.h>
#include <stdlib.h>

#include "asm.h"
#include "util.h"
#include "symtable.h"

#define SYMTABLE_SIZE 16

int
asm_assemble(FILE *fp, char *mem)
{

	symtable_entry_t *tab;

	// null ptr returned means symbol table couldn't be made
	if ( (tab = symtable_build(fp, SYMTABLE_SIZE)) == NULL) {
		die("Error building symbol table\n");
	}

	for (int i = 0; i < SYMTABLE_SIZE; i++) {
		printf("tab[%d].name = %s\n", i, tab[i].name);
	}

	symtable_cleanup(tab, SYMTABLE_SIZE);

	return EXIT_SUCCESS;

}
