#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "util.h"
#include "symtable.h"
#include "parseline.h"

/* 
 * Loop through 'fp' and keep track of the current address.
 * This will require reading opcodes and operands but not
 * adding them to the output file.
 */
size_t symtable_build(FILE *fp, struct symbol *symtable_head)
{
	int address = 0;
	char line[2048];
	while (fgets(line, sizeof(line), fp)) {
		size_t len = strlen(line);
		if ((len > 0) && (line[len - 1] != '\n')) {
			die("Line too long");
		}
		struct line_data data;
		address += parseline(line, &data);
	}
	return (size_t)address; // TODO return the actual size
}
