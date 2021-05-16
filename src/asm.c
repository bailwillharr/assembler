#include <stdio.h>
#include <stdlib.h>

#include "asm.h"
#include "util.h"
#include "symtable.h"

#define SYMTABLE_SIZE 1024

int
asm_assemble(FILE* fp, char* mem)
{

	// allocate space for the symtable
	symtable_entry_t *symbols =
		malloc(sizeof(symtable_entry_t) * SYMTABLE_SIZE);

	if (symtable_build(fp, &symbols, SYMTABLE_SIZE) != EXIT_SUCCESS) {
		die("Error building symbol table\n");
	}

	free(symbols);

	return EXIT_SUCCESS;

}
