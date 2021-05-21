#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "util.h"
#include "asm.h"

#define MEMORY_SIZE 65536

static void
help()
{
	printf("help\n");
}

static void
version()
{
	printf("v0.0 Bailey Harrison\n");
}

int
main(int argc, char **argv)
{

	if (argc <= 1) die("No file name");

	int o = 1; // the char index into the argument string
	int i;
	for (i = 1; *(argv[i]) == '-' && i < argc; o++) {
		switch (*(argv[i] + o)){
		case 0:		// go to next argument
			o = 0;
			i++;
			break;
		case 'h':
			help();
			return EXIT_SUCCESS;
		case 'v':
			version();
			return EXIT_SUCCESS;
		default:
			die("Invalid option");
		}
		if (i == argc) die("No filename provided");

	}

	if (argc > i + 1) die("Multiple files specified");



	// Generate binary machine code
	FILE *fp = fopen(argv[i], "r");
	if (!fp) die(strerror(errno));

	char memory[MEMORY_SIZE] = {};

	asm_assemble(fp, memory, MEMORY_SIZE);
	fclose(fp);



	// Write binary machine code to file
	fp = fopen("file.bin", "wb");
	if (!fp) die(strerror(errno));

	if (fwrite(memory, 1, MEMORY_SIZE, fp) != MEMORY_SIZE)
		die("Unable to write to output file");
	fclose(fp);

	return EXIT_SUCCESS;

}
