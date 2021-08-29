#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "instructions.h"
#include "util.h"
#include "parseline.h"

#define LABEL_MAX_LEN 32

// should a character be in a label or not
static int is_label_char_valid(char c)
{
	switch (c) {
	case '0'...'9':
		return 1;
	case 'A'...'Z':
		return 1;
	case '_':
		return 1;
	case 'a'...'z':
		return 1;
	}
	return  0;
}

// returns number of bytes used by the line's instruction
size_t parseline(char *line, struct line_data *data)
{

	// to define label, loop until TAB/LF/SP/: is encountered
	if (	line[0] != '\t' &&
		line[0] != '\n') {
		// there must be a label!
		
		if ( isdigit(line[0]) != 0 || is_label_char_valid(line[0]) == 0) {
			die("Label begins with invalid character");
		}

		int len = 0;
		for ( ;; ) {
			int char_type = is_label_char_valid(line[len]);
			if (char_type == 1) len++;
			if (char_type == 0) {

				if (	line[len] != '\t' &&
					line[len] != '\n' &&
					line[len] != ' ' &&
					line[len] != ':') {

					die("Label contains an invalid character");

				} else {
					break;
				}

			}
		}
		// 'len' is now the length of the label name (ex. \0)
		data->new_label = malloc(len + 1);
		for (int i = 0; i < len; i++) {
			data->new_label[i] = line[i];
		}
		data->new_label[len] = '\0'; // add terminating \0
	} else {
		// there is no label
		data->new_label = NULL; 
	}


	data->is_pseudo_op = 0;
	data->opcode_sz = 2;
	data->opcode = 0xED4B; // ld bc, (**)
	data->operand_sz = 2;
	data->operand_is_literal = 1;
	data->operand_literal = 0xBEEF;
	return data->opcode_sz + data->operand_sz;

}
