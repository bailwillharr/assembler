#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "util.h"
#include "symtable.h"
#include "assemble.h"

// constants
#define DEFAULT_OUTFILE_NAME "out.bin"

static void usage(char *argv0)
{
    fprintf(stderr, "usage: %s [-hv] [-o output_file] file\n", argv0);
}

static void version()
{
    fprintf(stderr, "v0.0 Bailey Harrison\n");
}

int main(int argc, char *argv[])
{

	char outfile_name[64] = DEFAULT_OUTFILE_NAME;

    if (argc <= 1) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    int o = 1; // the char index into the argument string
    int i;
    for (i = 1; *(argv[i]) == '-' && i < argc; o++) {
        switch (argv[i][o]){
        case 0:     // go to next argument
            o = 0;
            i++;
            break;
        case 'h':
            usage(argv[0]);
            exit(EXIT_SUCCESS);
        case 'v':
            version();
			exit(EXIT_SUCCESS);
		case 'o':
			if (argc < i + 3) die("No output filename provided");
			i++;
			strncpy(outfile_name, argv[i], 63);
			if (strlen(outfile_name) != strlen(argv[i])) {
				die("Output filename too long");
			}
			o = 0;
			i++;
			break;
        default:
			usage(argv[0]);
			exit(EXIT_FAILURE);
        }
        if (i == argc) die("No input filename provided");

    }

    if (argc > i + 1) die("Multiple files specified");

    // Generate binary machine code
    struct stat st; // structure holds file type information
    if (stat(argv[i], &st) != EXIT_SUCCESS) {
        die(strerror(errno)); // error if file doesn't exist
    }
    if (!S_ISREG(st.st_mode)) {
        die("A regular file must be specified"); // error if not normal file
    }
    FILE *fp = fopen(argv[i], "r");
    if (!fp) die(strerror(errno)); // error if unable to open file


    // FIRST PASS
#ifdef DEBUG
	printf("FIRST PASS\n");
#endif
    // symtable is a linked list of symbol:address pairs
    struct symbol *symtable_head = NULL;
    size_t memory_size; // holds the size of the output binary
    memory_size = symtable_build(fp, &symtable_head);

#ifdef DEBUG
	// print symbol table after first pass
	symtable_print(symtable_head);
#endif
    
    // SECOND PASS
#ifdef DEBUG
	printf("SECOND PASS\n");
#endif
    char* memory = calloc(memory_size, 1);
    if (memory == NULL) die("Failed calloc()");
    rewind(fp);
    assemble(fp, symtable_head, memory);
    fclose(fp);



    // Write binary machine code to file
    if (stat(outfile_name, &st) == EXIT_SUCCESS) {
        if (!S_ISREG(st.st_mode)) {
            die("A regular file must be specified");
        }
    }

    fp = fopen(outfile_name, "wb");
    if (!fp) die(strerror(errno));

    if (fwrite(memory, 1, memory_size, fp) != memory_size) {
        die("Unable to write to output file");
    }

    fclose(fp);
    free(memory);
    return 0;

}
