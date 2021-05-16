#include <stdio.h>
#include <stdlib.h>

#include "asm.h"
#include "symtable.h"

#define SYMTABLE_SIZE 1024

int
asm_assemble(FILE* fp, char* mem)
{

	int return_code = EXIT_SUCCESS;

	// allocate space for the symtable
	symtable_entry_t *symbols =
		malloc(sizeof(symtable_entry_t) * SYMTABLE_SIZE);

	if (symtable_build(fp, &symbols, SYMTABLE_SIZE) != EXIT_SUCCESS) {
		printf("Error building symbol table\n");
		return_code = EXIT_FAILURE;
	}

	free(symbols);

	return return_code;

}
