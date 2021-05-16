#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "asm.h"

char memory[1<<16];

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

	int return_code = EXIT_SUCCESS;

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
		if (i == argc) die("Enter a filename");

	}

	if (argc > i + 1) die("Only one filename can be supplied.");

	FILE* fp = fopen(argv[i], "r");
	if (!fp) die("Unable to open file");

	if (asm_assemble(fp, memory) != EXIT_SUCCESS) {
		printf("Assembler error\n");
		return_code = EXIT_FAILURE;
	}

	fclose(fp);

	return return_code;
}
