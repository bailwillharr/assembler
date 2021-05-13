#include <stdio.h>
#include <stdlib.h>

void
die(char *str)
{
	fprintf(stderr, "ERROR: %s\n", str);
	exit(EXIT_FAILURE);
}

void
help()
{
	printf("help\n");
}

void
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
			return 0;
		case 'v':
			version();
			return 0;
		default:
			die("Invalid option\n");
		}
		if (i == argc) die("Enter a filename");

	}

	if (argc > i + 1) die("Only one filename can be supplied.");

	return 0;
}
