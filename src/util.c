#include <stdio.h>
#include <stdlib.h>

void die(char *str)
{
	fprintf(stderr, "ERROR: %s\n", str);
	exit(EXIT_FAILURE);
}
