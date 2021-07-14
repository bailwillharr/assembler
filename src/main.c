#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "util.h"
#include "symtable.h"
#include "assemble.h"

const char *OUTFILE_NAME = "file.rom";

static void help(char *argv0)
{
	printf("usage: %s [-hv] file\n", argv0);
}

static void version()
{
	printf("v0.0 Bailey Harrison\n");
}

int main(int argc, char *argv[])
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
			help(argv[0]);
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
	struct stat st;
	if (stat(argv[i], &st) != EXIT_SUCCESS) {
		die(strerror(errno));
	}
	if (!S_ISREG(st.st_mode)) {
		die("A regular file must be specified");
	}
	FILE *fp = fopen(argv[i], "r");
	if (!fp) die(strerror(errno));


	// FIRST PASS
	// symtable works as a linked list of symbols
	struct symbol symtable_head;
	size_t memory_size;
	memory_size = symtable_build(fp, &symtable_head);

	char* memory = calloc(memory_size, 1);
	if (memory == NULL) die("Failed calloc()");
	rewind(fp);
	assemble(fp, memory);
	fclose(fp);



	// Write binary machine code to file
	if (stat(OUTFILE_NAME, &st) == EXIT_SUCCESS) {
		if (!S_ISREG(st.st_mode)) {
			die("A regular file must be specified");
		}
	}

	fp = fopen(OUTFILE_NAME, "wb");
	if (!fp) die(strerror(errno));

	if (fwrite(memory, 1, memory_size, fp) != memory_size) {
		die("Unable to write to output file");
	}

	fclose(fp);
	free(memory);
	return EXIT_SUCCESS;

}
