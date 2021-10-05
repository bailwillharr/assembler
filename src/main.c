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

static void usage(char *argv0)
{
    printf("usage: %s [-hv] file\n", argv0);
}

static void version()
{
    printf("v0.0 Bailey Harrison\n");
}

int main(int argc, char *argv[])
{

    if (argc <= 1) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    int o = 1; // the char index into the argument string
    int i;
    for (i = 1; *(argv[i]) == '-' && i < argc; o++) {
        switch (*(argv[i] + o)){
        case 0:     // go to next argument
            o = 0;
            i++;
            break;
        case 'h':
            usage(argv[0]);
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
