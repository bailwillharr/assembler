#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "symtable.h"
#include "srcfile.h"

#define SYMBOL_NAME_SIZE 64
#define SYMBOL_VALUE_STR_SIZE 16

static void
symbol_from_line(symtable_entry_t *symbol, char *line)
{

	//Symbol definitions look like this: =NAME 123

	if (line[0] == '=') {

		char symbol_name[SYMBOL_NAME_SIZE] = {};
		char symbol_value_buf[SYMBOL_VALUE_STR_SIZE] = {};
		int symbol_value;

		int i; // the index into the line

		// read the symbol name
		for (i = 1; (i < SYMBOL_NAME_SIZE) &&
				(line[i] != 0) &&
				(line[i] != ' '); i++) {
			symbol_name[i - 1] = line[i];
		}

		// read the symbol value
		for (int j = 0; (line[i] != 0) &&
				(j < SYMBOL_VALUE_STR_SIZE); j++, i++) {
			symbol_value_buf[j] = line[i];
		}
		symbol_value = atoi(symbol_value_buf);

		// allocate space in RAM for the string
		symbol->name = malloc(SYMBOL_NAME_SIZE);
		if (symbol->name == NULL)
			die("Unable to allocate memory for symbol->name");

		strcpy(symbol->name, symbol_name);
		symbol->value = symbol_value;

	}

	// symbol->name is already null due to initialisation

}

symtable_entry_t *
symtable_build(FILE* fp, size_t sz)
{
	// calloc() is used to initialize all symbol entries to NULL
	symtable_entry_t *tab = calloc(sz, sizeof(symtable_entry_t));
	if (tab == NULL)
		die("Unable to allocate memory for symtable");

	char line[SRCFILE_LINE_LENGTH];
	int i = 0;

	// loop until it has reached EOF or the end of the symtable
	while ( fgets(line, sizeof(line), fp) ) {

		if (i >= sz)
			die(
			"Too many symbols have been defined.\n"
			"Consider removing symbols or increasing "
			"SYMTABLE_SIZE.");

		symbol_from_line(&tab[i], line);
		if (tab[i].name != NULL)	// if a symbol was read
			i++;

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
