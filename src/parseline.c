#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "instructions.h"
#include "util.h"
#include "parseline.h" 

#define LABEL_MAX_LEN 16
#define OPERAND_NAME_MAX_LEN 32






struct ParsedOperand {
	bool isIndirect;			// does the operand use (  ) ?
	char reg[LABEL_MAX_LEN];	// can also be a flag or label; if IX/IY, 'value' becomes an offset
	signed int value;
};

struct ParsedInstruction {
	char opcode[4];
	struct ParsedOperand operand1, operand2;
};

static void instruction_parse(const char *opcode, char *operands, struct ParsedInstruction *inst)
{

	printf("%s\n", opcode);

}











/* modifies the following members of 'data':
 * ->opcode_sz
 * ->opcode
 * ->pseudo_op
 * ->operand_sz
 * ->operand_label
 * ->operand_literal
 */
static void instruction_lookup(const char *opcode_name, char *operand_name, struct line_data *data) {

	// first narrow down based on opcode_name
	
#ifdef DEBUG
	printf("opcode_name: %s, operand_name: %s\n", opcode_name, operand_name);
#endif

	data->opcode_sz = 0;
	data->operand_sz = 0;

	switch (opcode_name[0]) {
/*		case 'x':
			data->opcode_sz = 1;
			data->opcode[0] = XOR_A;
			break;
		case 'j':
			switch (opcode_name[1]) {
				case 'r':
					data->opcode_sz = 1;
					data->opcode[0] = JR_N;
					data->operand_sz = 1;
					data->operand_label = operand_name;
					break;
			}
			break;
*/
		case '.':
			// PSEUDO OPS
			if (strcmp(opcode_name + 1, "org") == 0) {

				printf(":)\n");

				uint16_t newAddr;
				sscanf(operand_name, "$%hX", &newAddr);

				// -1 means pseudo opcode
				data->opcode_sz = -1;
				data->pseudo_op = PSEUDO_ORG;
				data->operand_sz = 2;
				data->operand_label = NULL;
				data->operand_literal = newAddr;
			}
			break;
		default:
			{	// NORMAL INSTRUCTION
				struct ParsedInstruction instruction;
				instruction_parse(opcode_name, operand_name, &instruction);
			}
			break;
		}

}

static inline bool is_operand_char_valid(char c) {
	if (isalnum(c)) return true;
	switch (c) {
	case ' ':
	case '+':
	case '-':
	case '_':
	case '(':
	case ')':
	case ',':
	case '$':
	case '%':
	case '\t':
		return true;
	}
	return false;
}

// should a character be in a label or not
static inline bool is_label_char_valid(char c)
{
	// label can only contain alphanumerics and underscores
	if (isalnum(c)) return true;
	if (c == '_') return true;
	return false;
}

// should a character be in an opcode
static inline bool is_opcode_char_valid(char c)
{
	// opcode can only contain letters (uppercase or lowercase)
	if (isalpha(c)) return true;
	if (c == '.') return true;
	else return false;
}	

// returns number of bytes used by the line's instruction
size_t parseline(char *line, struct line_data *data, int line_no)
{

	data->new_label = NULL;
	data->opcode_sz = 0;
	data->operand_sz = 0;
	data->operand_label = NULL;

	if (line[0] == ';') return 0;

	int index = 0; // index into line

	// BEGIN LABEL PARSING
	// to define label, loop until TAB/LF/: is encountered
	if (line[0] != '\t' &&
		line[0] != '\n') {
		// there is a label on this line
		
		if ( isdigit(line[0]) != 0 || is_label_char_valid(line[0]) == 0) {
			fprintf(stderr, "line %d: Label begins with invalid character\n", line_no);
			exit(-1);
		}

		int len = 0;
		for ( ;; ) {
			if (line[len] == ';')  break;
			int char_type = is_label_char_valid(line[len]);
			if (char_type == 1) len++;
			if (char_type == 0) {

				if (	line[len] != '\t' &&
					line[len] != '\n' &&
					line[len] != ' ' &&
					line[len] != ':') {

					fprintf(stderr, "line %d: Label contains an invalid character\n", line_no);
					exit(EXIT_FAILURE);
	
				} else {
					break;
				}

			}
		}
		// TODO: instead of malloc, return the length of substring in current line, we know it always starts at column zero.
		// This means malloc() is only called during the first pass and not the second

		// 'len' is now the length of the label name (ex. \0)

		if (len > LABEL_MAX_LEN) {
			fprintf(stderr, "line %d: label name too long\n", line_no);
		}

		data->new_label = malloc(len + 1);
		for (int i = 0; i < len; i++) {
			data->new_label[i] = line[i];
		}
		data->new_label[len] = 0;

#ifdef DEBUG
		fprintf(stderr, "label '%s' defined on line %d\n", data->new_label, line_no);
#endif

		data->new_label[len] = '\0'; // add terminating 0

		index += len;

		// skip over optional colon
		if (line[index] == ':') {
			index++;
		}

	}
	// LABEL PARSING DONE

	// skip until opcode
	for ( ; line[index] == '\t' || line[index] == ' '; index++) ;

	if (line[index] == '\n') return 0;

	// BEGIN OPCODE PARSING
	int len = 0; // length of opcode name
	char opcode_name[OPCODE_NAME_MAX_LEN + 1] = { 0 };
	for ( ;; ) {
		if (line[index] == ';') break;
		int char_valid = is_opcode_char_valid(line[index]);
		if (char_valid == 1) {
			// char is valid
			opcode_name[len] = line[index];
			len++;
			index++;
			if (len > OPCODE_NAME_MAX_LEN) {
				fprintf(stderr, "line %d: Opcode name too long\n", line_no);
				exit(EXIT_FAILURE);
			}
		} else {
			// char is not valid or is end of opcode
			if (	line[index] != '\n' &&
				line[index] != '\t' &&
				line[index] != ' '	) {
				fprintf(stderr, "line %d: Opcode contains invalid characters, char: %2X\n", line_no, line[index]);
				exit(EXIT_FAILURE);
			} else break;
		}
	}
	if (strlen(opcode_name) == 0) return 0;

	// END OPCODE PARSE

	for ( ; line[index] == '\t' || line[index] == ' '; index++) ;

	// BEGIN OPERAND PARSE
	// any characters until newline excluding anything after a comment.

	int operand_str_size = 0;
	char *start_of_operand = line + index;
	for ( ;; ) {
		if (line[index] == ';') break;
		int char_valid = is_operand_char_valid(line[index]);
		if (char_valid == 1) {
			operand_str_size++;
			index++;
		}
		if (char_valid == 0) {
			if (line[index] != '\n') {
				fprintf(stderr, "ERROR: line %d: operand contains invalid character: %2X (hex)\n", line_no, line[index]);
				exit(EXIT_FAILURE);
			} else break;
		}
	}
	char *operand_str = NULL;
	if (operand_str_size != 0) {
		operand_str = malloc(operand_str_size + 1);
		memcpy(operand_str, start_of_operand, operand_str_size);
		operand_str[operand_str_size] = 0; // null terminator
	}

	if (operand_str_size > OPERAND_NAME_MAX_LEN) {
		fprintf(stderr, "on line %d: operand too long\n", line_no);
	}

	// END OPERAND PARSE

	// convert opcode and operands to all lowercase
	for (int i = 0; i < OPCODE_NAME_MAX_LEN; i++) {
		opcode_name[i] = tolower(opcode_name[i]);
	}
	for (int i = 0; i < operand_str_size; i++) {
		operand_str[i] = tolower(operand_str[i]);
	}

	// figure out what the instruction is
	instruction_lookup(opcode_name, operand_str, data);

#ifdef DEBUG
	printf("OPERAND_LABEL: %s\n", data->operand_label);
	printf("OPERAND_LITERAL: %d\n", data->operand_literal);
#endif

	if (data->opcode_sz == -1) { // pseudo opcode
		return 0;
	}
	return data->opcode_sz + data->operand_sz;

}
