#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "instructions.h"
#include "util.h"
#include "parseline.h"

// should a character be in a label or not
static inline int is_label_char_valid(char c)
{
	// label can only contain alphanumerics and underscores
	if (isalnum(c)) return 1;
	if (c == '_') return 1;
	return  0;
}

// should a character be in an opcode or not
static inline int is_opcode_char_valid(char c)
{
	// opcode can only contain letters (uppercase or lowercase)
	return isalpha(c);
}

// returns number of bytes used by the line's instruction
size_t parseline(char *line, struct line_data *data)
{

	data->new_label = NULL;
	data->pseudo_op = PSEUDO_NULL;
	data->opcode_sz = 0;
	data->opcode = 0;
	data->operand_sz = 0;
	data->operand_label = NULL;
	data->operand_literal = 0;

	int index = 0; // index into line



	// BEGIN LABEL PARSING
	// to define label, loop until TAB/LF/: is encountered
	if (	line[0] != '\t' &&
		line[0] != '\n') {
		// there is a label on this line
		
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

		// DEBUG
		printf("label: %s has length: %d\n", data->new_label, len);

		data->new_label[len] = '\0'; // add terminating 0

		index += len;

		// skip over optional colon
		if (line[index] == ':') {
			// DEBUG
			printf("COLON\n");
			index++;
		}

	}
	// LABEL PARSING DONE

	if (line[index] == '\n') return 0;

	// skip until opcode or pseudo opcode:
	for ( ; line[index] == '\t' || line[index] == ' '; index++) ;

	// BEGIN OPCODE PARSING
	int len = 0; // length of opcode name
	char opcode_name[OPCODE_NAME_MAX_LEN] = { 0 };
	for ( ;; ) {
		if (is_opcode_char_valid(line[index]) != 0) {
			// char is valid
			opcode_name[len] = line[index];
			len++;
			index++;
			if (len > OPCODE_NAME_MAX_LEN) {
				die("Opcode name too long");
			}
		} else {
			// char is not valid or is end of opcode
			if (	line[index] != '\n' &&
				line[index] != '\t' &&
				line[index] != ' ') {
				die ("Opcode contains invalid characters");
			} else {
				break;
			}
		}
	}
	// DEBUG
	printf("op: %s\n", opcode_name);
	size_t opcode_sz;
	int opcode;
	opcode_lookup(opcode_name, &opcode, &opcode_sz);

	return data->opcode_sz + data->operand_sz;

}
