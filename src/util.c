#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void die(const char *format, ...) {
	fprintf(stderr, "ERROR: ");
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}
