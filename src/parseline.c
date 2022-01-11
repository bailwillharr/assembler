#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "util.h"
#include "parseline.h" 

#define OPERAND_NAME_MAX_LEN 32



// all lowercase
struct ParsedOperand {
	bool isIndirect;			// does the operand use (  ) ?
	char reg[LABEL_MAX_LEN+1];	// can also be a flag or label; if IX/IY, 'value' becomes an offset
	signed int value;
};

static struct ParsedOperand operand_parse(const char *arg)
{

	struct ParsedOperand po = { 0 };

	const char* lbkt_loc = strchr(arg, '(');
	const char* rbkt_loc = strchr(arg, ')');
	if (lbkt_loc != NULL && rbkt_loc != NULL) {
		// operand has brackets
		po.isIndirect = true;

		if (lbkt_loc > rbkt_loc) {
			die("mismatched brackets with operand: %s", arg);
		}
	} else {
		po.isIndirect = false;
	}

	const char *reg_start = NULL;
	for (int i = 0; arg[i] != '\0'; i++) {
		// first find start of label/reg/flag, if it exists
		if ( (isalpha(arg[i]) || arg[i] == '_')) {
			reg_start = arg + i;
			break;
		}
	}

	// now get value/offset
	const char *num_start = NULL;
	for (int i = 0; arg[i] != '\0'; i++) {
		if (isdigit(arg[i]) || arg[i] == '+' || arg[i] == '-') {
			num_start = arg + i;
			break;
		}
	}

	if (num_start != NULL && num_start < reg_start) reg_start = NULL;

	if (reg_start != NULL) {
		// there is a label/reg/flag

		// get the entire reg/label/flag string
		for (int i = 0; i < LABEL_MAX_LEN; i++) {
			if (isalnum(reg_start[i]) || reg_start[i] == '_') {
				po.reg[i] = reg_start[i];
			} else {
				po.reg[i] = '\0';
				break;
			}
		}
	} else {
		po.reg[0] = '\0';
	}

	if (num_start != NULL) {
		// is there is a val/offset
		
		// copy it into buffer
		char num_buffer[16] = { 0 };
		for (int i = 0; i < 15; i++) {
			if ( isdigit(num_start[i]) || ('a' <= tolower(num_start[i]) && tolower(num_start[i]) <= 'f') || ( ( i == 1 || i == 2 ) && tolower(num_start[i]) == 'x') ||
					( i == 0 && ( (num_start[i] == '+') || (num_start[i] == '-') ) )
					  ) { // 'x' is in case of hexadecimal
				num_buffer[i] = num_start[i];
			} else {
				num_buffer[i] = '\0';
				break;
			}
		}

		// convert it to integer
		int num = strtol(num_buffer, NULL, 0); // 0 base means either 10, 8, or 16 depending on input
		po.value = (signed int)num;
		
	} else {
		po.value = 0;
	}


	return po;
}

struct ParsedInstruction {
	char opcode[OPCODE_NAME_MAX_LEN+1];
	int operands;
	struct ParsedOperand operand1, operand2;
};

static struct ParsedInstruction instruction_parse(const char *opcode, char *operands)
{

	struct ParsedInstruction inst;

#ifndef NDEBUG
	fprintf(stderr, "\n\t%s\t%s\n", opcode, operands);
#endif

	for (int i = 0; i < OPCODE_NAME_MAX_LEN+1; i++) {
		inst.opcode[i] = tolower(opcode[i]);
		if (opcode[i] == '\0') {
			inst.opcode[i] = '\0';
			break;
		}
	}

	if (operands == NULL) {
		inst.operands = 0;
		return inst;
	}

	// operands

	char args[OPERAND_NAME_MAX_LEN+2];
	strncpy(args, operands, OPERAND_NAME_MAX_LEN+1);

	char *arg2 = NULL;

	for (int i = 0; args[i] != '\0'; i++) {

		// convert tabs to spaces
		if (args[i] == '\t') args[i] = ' ';

		// find if there is a comma, and replace with '\0' to split operands
		if (args[i] == ',') {
			args[i] = '\0';
			arg2 = args + i + 1;
		}
		
	}

	char *arg1 = args;

	inst.operand1 = operand_parse(arg1);
	if (arg2 != NULL) {
		inst.operand2 = operand_parse(arg2);
		inst.operands = 2;
	} else {
		inst.operands = 1;
	}

	return inst;

}

static bool label_is_reserved(const char *lbl)
{
	const char *RESERVED[] = {
		"a", "f", "b", "c", "d", "e", "h", "l",
		"af", "bc", "de", "hl",
		"i", "r", "ir", "sp", "pc",
		"ix", "iy", "ixh", "ixl", "iyh", "iyl",
		"z", "c", "m", "pe", "nz", "nc", "po", "p"
	};
	for (int i = 0; i < sizeof(RESERVED) / sizeof(const char *); i++) {
		if (strcmp(RESERVED[i], lbl) == 0) return true;
	}
	return false;
}

struct DecodedInstruction {
	size_t opcode_sz;
	uint8_t opcode[3]; // start at [0]
	size_t operand_sz; // if 0, no operand
	char operand_label[LABEL_MAX_LEN+1]; // if empty, use literal
	uint16_t operand_literal;
};

struct InstructionDecodeEntry {
	// result
	size_t	operand_sz;

	// pattern to match
	const char 	   *p_instruction_name;
	int				p_operand_count;
	// operand 1
	bool			p_op1_indirect;
	const char		p_op1_reg[LABEL_MAX_LEN+1];
	// operand 2
	bool			p_op2_indirect;
	const char		p_op2_reg[LABEL_MAX_LEN+1];
};

const struct InstructionDecodeEntry DECODE_TABLE_MAIN[] = {
	// 0x00
	{	0,	"nop",	0,								},
	{	2,	"ld",	2,	false,	"bc",	false,	""	},
	{	0,	"ld",	2,	true,	"bc",	false,	"a"	},
	{	0,	"inc",	1,	false,	"bc",				},
	{	0,	"inc",	1,	false,	"b",				},
	{	0,	"dec",	1,	false,	"b",				},
	{	1,	"ld",	2,	false,	"b",	false,	""	},
	{	0,	"rlca",	0,								},
	{	0,	"ex",	2,	false,	"af",	false,	"af"},
	{	0,	"add",	2,	false,	"hl",	false,	"bc"},
	{	0,	"ld",	2,	false,	"a",	true,	"bc"},
	{	0,	"dec",	1,	false,	"bc",				},
	{	0,	"inc",	1,	false,	"c",				},
	{	0,	"dec",	1,	false,	"c",				},
	{	1,	"ld",	2,	false,	"c",	false,	""	},
	{	0,	"rrca",	0,								},
	// 0x10
	{	1,	"djnz",	1,	false,	"",					},
	{	2,	"ld",	2,	false,	"de",	false,	""	},
	{	0,	"ld",	2,	true,	"de",	false,	"a"	},
	{	0,	"inc",	1,	false,	"de",				},
	{	0,	"inc",	1,	false,	"d",				},
	{	0,	"dec",	1,	false,	"d",				},
	{	1,	"ld",	2,	false,	"d",	false,	""	},
	{	0,	"rla",	0,								},
	{	1,	"jr",	1,	false,	"",					},
	{	0,	"add",	2,	false,	"hl",	false,	"de"},
	{	0,	"ld",	2,	false,	"a",	true,	"de"},
	{	0,	"dec",	1,	false,	"de",				},
	{	0,	"inc",	1,	false,	"e",				},
	{	0,	"dec",	1,	false,	"e",				},
	{	1,	"ld",	2,	false,	"e",	false,	""	},
	{	0,	"rra",	0,								},
	// 0x20
	{	1,	"jr",	2,	false,	"nz",	false,	""	},
	{	2,	"ld",	2,	false,	"hl",	false,	""	},
	{	2,	"ld",	2,	true,	"",		false,	"hl"},
	{	0,	"inc",	1,	false,	"hl",				},
	{	0,	"inc",	1,	false,	"h",				},
	{	0,	"dec",	1,	false,	"h",				},
	{	1,	"ld",	2,	false,	"h",	false,	""	},
	{	0,	"daa",	0,								},
	{	1,	"jr",	2,	false,	"z",	false,	""	},
	{	0,	"add",	2,	false,	"hl",	false,	"hl"},
	{	2,	"ld",	2,	false,	"hl",	true,	""	},
	{	0,	"dec",	1,	false,	"hl",				},
	{	0,	"inc",	1,	false,	"l",				},
	{	0,	"dec",	1,	false,	"l",				},
	{	1,	"ld",	2,	false,	"l",	false,	""	},
	{	0,	"cpl",	0,								},
	// 0x30
	{	1,	"jr",	2,	false,	"nc",	false,	""	},
	{	2,	"ld",	2,	false,	"sp",	false,	""	},
	{	2,	"ld",	2,	true,	"",		false,	"a"	},
	{	0,	"inc",	1,	false,	"sp",				},
	{	0,	"inc",	1,	true,	"hl",				},
	{	0,	"dec",	1,	true,	"hl",				},
	{	1,	"ld",	2,	true,	"hl",	false,	""	},
	{	0,	"scf",	0,								},
	{	1,	"jr",	2,	false,	"c",	false,	""	},
	{	0,	"add",	2,	false,	"hl",	false,	"sp"},
	{	2,	"ld",	2,	false,	"a",	true,	""	},
	{	0,	"dec",	1,	false,	"sp",				},
	{	0,	"inc",	1,	false,	"a",				},
	{	0,	"dec",	1,	false,	"a",				},
	{	1,	"ld",	2,	false,	"a",	false,	""	},
	{	0,	"ccf",	0,								},
	// 0x40
	{	0,	"ld",	2,	false,	"b",	false,	"b"	},
	{	0,	"ld",	2,	false,	"b",	false,	"c"	},
	{	0,	"ld",	2,	false,	"b",	false,	"d"	},
	{	0,	"ld",	2,	false,	"b",	false,	"e"	},
	{	0,	"ld",	2,	false,	"b",	false,	"h"	},
	{	0,	"ld",	2,	false,	"b",	false,	"l"	},
	{	0,	"ld",	2,	false,	"b",	true,	"hl"},
	{	0,	"ld",	2,	false,	"b",	false,	"a"	},
	{	0,	"ld",	2,	false,	"c",	false,	"b"	},
	{	0,	"ld",	2,	false,	"c",	false,	"c"	},
	{	0,	"ld",	2,	false,	"c",	false,	"d"	},
	{	0,	"ld",	2,	false,	"c",	false,	"e"	},
	{	0,	"ld",	2,	false,	"c",	false,	"h"	},
	{	0,	"ld",	2,	false,	"c",	false,	"l"	},
	{	0,	"ld",	2,	false,	"c",	true,	"hl"},
	{	0,	"ld",	2,	false,	"c",	false,	"a"	},
	// 0x50
	{	0,	"ld",	2,	false,	"d",	false,	"b"	},
	{	0,	"ld",	2,	false,	"d",	false,	"c"	},
	{	0,	"ld",	2,	false,	"d",	false,	"d"	},
	{	0,	"ld",	2,	false,	"d",	false,	"e"	},
	{	0,	"ld",	2,	false,	"d",	false,	"h"	},
	{	0,	"ld",	2,	false,	"d",	false,	"l"	},
	{	0,	"ld",	2,	false,	"d",	true,	"hl"},
	{	0,	"ld",	2,	false,	"d",	false,	"a"	},
	{	0,	"ld",	2,	false,	"e",	false,	"b"	},
	{	0,	"ld",	2,	false,	"e",	false,	"c"	},
	{	0,	"ld",	2,	false,	"e",	false,	"d"	},
	{	0,	"ld",	2,	false,	"e",	false,	"e"	},
	{	0,	"ld",	2,	false,	"e",	false,	"h"	},
	{	0,	"ld",	2,	false,	"e",	false,	"l"	},
	{	0,	"ld",	2,	false,	"e",	true,	"hl"},
	{	0,	"ld",	2,	false,	"e",	false,	"a"	},
	//	0x60
	{	0,	"ld",	2,	false,	"h",	false,	"b"	},
	{	0,	"ld",	2,	false,	"h",	false,	"c"	},
	{	0,	"ld",	2,	false,	"h",	false,	"d"	},
	{	0,	"ld",	2,	false,	"h",	false,	"e"	},
	{	0,	"ld",	2,	false,	"h",	false,	"h"	},
	{	0,	"ld",	2,	false,	"h",	false,	"l"	},
	{	0,	"ld",	2,	false,	"h",	true,	"hl"},
	{	0,	"ld",	2,	false,	"h",	false,	"a"	},
	{	0,	"ld",	2,	false,	"l",	false,	"b"	},
	{	0,	"ld",	2,	false,	"l",	false,	"c"	},
	{	0,	"ld",	2,	false,	"l",	false,	"d"	},
	{	0,	"ld",	2,	false,	"l",	false,	"e"	},
	{	0,	"ld",	2,	false,	"l",	false,	"h"	},
	{	0,	"ld",	2,	false,	"l",	false,	"l"	},
	{	0,	"ld",	2,	false,	"l",	true,	"hl"},
	{	0,	"ld",	2,	false,	"l",	false,	"a"	},
	// 0x70
	{	0,	"ld",	2,	true,	"hl",	false,	"b"	},
	{	0,	"ld",	2,	true,	"hl",	false,	"c"	},
	{	0,	"ld",	2,	true,	"hl",	false,	"d"	},
	{	0,	"ld",	2,	true,	"hl",	false,	"e"	},
	{	0,	"ld",	2,	true,	"hl",	false,	"h"	},
	{	0,	"ld",	2,	true,	"hl",	false,	"l"	},
	{	0,	"halt",	0,								},
	{	0,	"ld",	2,	true,	"hl",	false,	"a"	},
	{	0,	"ld",	2,	false,	"a",	false,	"b"	},
	{	0,	"ld",	2,	false,	"a",	false,	"c"	},
	{	0,	"ld",	2,	false,	"a",	false,	"d"	},
	{	0,	"ld",	2,	false,	"a",	false,	"e"	},
	{	0,	"ld",	2,	false,	"a",	false,	"h"	},
	{	0,	"ld",	2,	false,	"a",	false,	"l"	},
	{	0,	"ld",	2,	false,	"a",	true,	"hl"},
	{	0,	"ld",	2,	false,	"a",	false,	"a"	},
	// 0x80
	{	0,	"add",	2,	false,	"a",	false,	"b"	},
	{	0,	"add",	2,	false,	"a",	false,	"c"	},
	{	0,	"add",	2,	false,	"a",	false,	"d"	},
	{	0,	"add",	2,	false,	"a",	false,	"e"	},
	{	0,	"add",	2,	false,	"a",	false,	"h"	},
	{	0,	"add",	2,	false,	"a",	false,	"l"	},
	{	0,	"add",	2,	false,	"a",	true,	"hl"},
	{	0,	"add",	2,	false,	"a",	false,	"a"	},
	{	0,	"adc",	2,	false,	"a",	false,	"b"	},
	{	0,	"adc",	2,	false,	"a",	false,	"c"	},
	{	0,	"adc",	2,	false,	"a",	false,	"d"	},
	{	0,	"adc",	2,	false,	"a",	false,	"e"	},
	{	0,	"adc",	2,	false,	"a",	false,	"h"	},
	{	0,	"adc",	2,	false,	"a",	false,	"l"	},
	{	0,	"adc",	2,	false,	"a",	true,	"hl"},
	{	0,	"adc",	2,	false,	"a",	false,	"a"	},
	// 0x90
	{	0,	"sub",	1,	false,	"b",				},
	{	0,	"sub",	1,	false,	"c",				},
	{	0,	"sub",	1,	false,	"d",				},
	{	0,	"sub",	1,	false,	"e",				},
	{	0,	"sub",	1,	false,	"h",				},
	{	0,	"sub",	1,	false,	"l",				},
	{	0,	"sub",	1,	true,	"hl",				},
	{	0,	"sub",	1,	false,	"a",				},
	{	0,	"sdc",	2,	false,	"a",	false,	"b"	},
	{	0,	"sdc",	2,	false,	"a",	false,	"c"	},
	{	0,	"sdc",	2,	false,	"a",	false,	"d"	},
	{	0,	"sdc",	2,	false,	"a",	false,	"e"	},
	{	0,	"sdc",	2,	false,	"a",	false,	"h"	},
	{	0,	"sdc",	2,	false,	"a",	false,	"l"	},
	{	0,	"sdc",	2,	false,	"a",	true,	"hl"},
	{	0,	"sdc",	2,	false,	"a",	false,	"a"	},
	// 0xA0
	{	0,	"and",	1,	false,	"b",				},
	{	0,	"and",	1,	false,	"c",				},
	{	0,	"and",	1,	false,	"d",				},
	{	0,	"and",	1,	false,	"e",				},
	{	0,	"and",	1,	false,	"h",				},
	{	0,	"and",	1,	false,	"l",				},
	{	0,	"and",	1,	true,	"hl",				},
	{	0,	"and",	1,	false,	"a",				},
	{	0,	"xor",	1,	false,	"b",				},
	{	0,	"xor",	1,	false,	"c",				},
	{	0,	"xor",	1,	false,	"d",				},
	{	0,	"xor",	1,	false,	"e",				},
	{	0,	"xor",	1,	false,	"h",				},
	{	0,	"xor",	1,	false,	"l",				},
	{	0,	"xor",	1,	true,	"hl",				},
	{	0,	"xor",	1,	false,	"a",				},
	// 0xB0
	{	0,	"or",	1,	false,	"b",				},
	{	0,	"or",	1,	false,	"c",				},
	{	0,	"or",	1,	false,	"d",				},
	{	0,	"or",	1,	false,	"e",				},
	{	0,	"or",	1,	false,	"h",				},
	{	0,	"or",	1,	false,	"l",				},
	{	0,	"or",	1,	true,	"hl",				},
	{	0,	"or",	1,	false,	"a",				},
	{	0,	"cp",	1,	false,	"b",				},
	{	0,	"cp",	1,	false,	"c",				},
	{	0,	"cp",	1,	false,	"d",				},
	{	0,	"cp",	1,	false,	"e",				},
	{	0,	"cp",	1,	false,	"h",				},
	{	0,	"cp",	1,	false,	"l",				},
	{	0,	"cp",	1,	true,	"hl",				},
	{	0,	"cp",	1,	false,	"a",				},
	// 0xC0
	{	0,	"ret",	1,	false,	"nz",				},
	{	0,	"pop",	1,	false,	"bc",				},
	{	2,	"jp",	2,	false,	"nz",	false,	""	},
	{	2,	"jp",	1,	false,	"",					},
	{	2,	"call",	2,	false,	"nz",	false,	""	},
	{	0,	"push",	1,	false,	"bc",				},
	{	1,	"add",	2,	false,	"a",	false,	""	},
	{	0,	"rst",	1,	false,	"0x00",				},
	{	0,	"ret",	1,	false,	"z",				},
	{	0,	"ret",	0,								},
	{	2,	"jp",	2,	false,	"z",	false,	""	},
	{	0,	"",		0,								},
	{	2,	"call",	2,	false,	"z",	false,	""	},
	{	2,	"call",	1,	false,	"",					},
	{	1,	"adc",	2,	false,	"a",	false,	""	},
	{	0,	"rst",	1,	false,	"0x08",				},
	// 0xD0
	{	0,	"ret",	1,	false,	"nc",				},
	{	0,	"pop",	1,	false,	"de",				},
	{	2,	"jp",	2,	false,	"nc",	false,	""	},
	{	1, 	"out",	2,	true,	"",		false,	"a"	},
	{	2,	"call",	2,	false,	"nc",	false,	""	},
	{	0,	"push",	1,	false,	"de",				},
	{	1,	"sub",	1,	false,	"",					},
	{	0,	"rst",	1,	false,	"0x10",				},
	{	0,	"ret",	1,	false,	"c",				},
	{	0,	"exx",	0,								},
	{	2,	"jp",	2,	false,	"c",	false,	""	},
	{	1,	"in",	2,	false,	"a",	true,	""	},
	{	2,	"call",	2,	false,	"c",	false,	""	},
	{	0,	"",		0,								},
	{	1,	"sbc",	2,	false,	"a",	false,	""	},
	{	0,	"rst",	1,	false,	"0x18",				},
	// 0xE0
	{	0,	"ret",	1,	false,	"po",				},
	{	0,	"pop",	1,	false,	"hl",				},
	{	2,	"jp",	2,	false,	"po",	false,	""	},
	{	0, 	"ex",	2,	true,	"sp",	false,	"hl"},
	{	2,	"call",	2,	false,	"po",	false,	""	},
	{	0,	"push",	1,	false,	"hl",				},
	{	1,	"and",	1,	false,	"",					},
	{	0,	"rst",	1,	false,	"0x20",				},
	{	0,	"ret",	1,	false,	"pe",				},
	{	0,	"jp",	1,	true,	"hl",				},
	{	2,	"jp",	2,	false,	"pe",	false,	""	},
	{	0,	"ex",	2,	false,	"de",	false,	"hl"},
	{	2,	"call",	2,	false,	"pe",	false,	""	},
	{	0,	"",		0,								},
	{	1,	"xor",	1,	false,	"",					},
	{	0,	"rst",	1,	false,	"0x28",				},
	// 0xF0
	{	0,	"ret",	1,	false,	"p",				},
	{	0,	"pop",	1,	false,	"af",				},
	{	2,	"jp",	2,	false,	"p",	false,	""	},
	{	0, 	"di",	0,								},
	{	2,	"call",	2,	false,	"p",	false,	""	},
	{	0,	"push",	1,	false,	"af",				},
	{	1,	"or",	1,	false,	"",					},
	{	0,	"rst",	1,	false,	"0x30",				},
	{	0,	"ret",	1,	false,	"m",				},
	{	0,	"ld",	2,	false,	"sp",	false,	"hl"},
	{	2,	"jp",	2,	false,	"m",	false,	""	},
	{	0,	"ei",	0,								},
	{	2,	"call",	2,	false,	"m",	false,	""	},
	{	0,	"",		0,								},
	{	1,	"cp",	1,	false,	"",					},
	{	0,	"rst",	1,	false,	"0x38",				},

};

static struct DecodedInstruction instruction_decode(struct ParsedInstruction p)
{

	struct DecodedInstruction d;
	d.opcode_sz = 0; // if this remains 0, the opcode was not found

	for (int i = 0; i < (int)(sizeof(DECODE_TABLE_MAIN) / sizeof(struct InstructionDecodeEntry)); i++) {
		const struct InstructionDecodeEntry e = DECODE_TABLE_MAIN[i];
		if (strcmp(e.p_instruction_name, p.opcode) == 0) {
			if (e.p_operand_count == p.operands) {
				if (e.p_operand_count == 0) {
					d.opcode_sz = 1;
					d.opcode[0] = i;
					d.operand_sz = 0;
					d.operand_label[0] = 0;
					break;
				}
				if (e.p_operand_count >= 1) {

					if (e.p_op1_indirect != p.operand1.isIndirect) continue;
					if (e.p_operand_count == 2 && (e.p_op2_indirect != p.operand2.isIndirect) ) continue;

					if (e.p_operand_count == 1) {
						if (e.p_op1_reg[0] == 0) {
							// op1 value is definitely operand
							if (p.operand1.reg[0] == 0) {
								d.operand_label[0] = 0;
								d.operand_literal = p.operand1.value;
							} else {
								strncpy(d.operand_label, p.operand1.reg, LABEL_MAX_LEN+1);
							}
							d.opcode_sz = 1;
							d.opcode[0] = i;
							d.operand_sz = e.operand_sz;
							break;
						} else if (strcmp(e.p_op1_reg, p.operand1.reg) == 0) {
							// match, no final operand
							d.opcode_sz = 1;
							d.opcode[0] = i;
							d.operand_sz = 0;
							d.operand_label[0] = 0;
							break;
						} else continue;
					}

					if (e.p_operand_count == 2) {
						// three possible matching conditions:
						// MATCH,	LBL_VAL
						// LBL/VAL,	MATCH
						// MATCH,	MATCH
					
						if ( e.p_op1_reg[0] != 0 && e.p_op2_reg[0] != 0 ) {
							if (strcmp(e.p_op1_reg, p.operand1.reg) != 0) {
								continue;
							}
							if (strcmp(e.p_op2_reg, p.operand2.reg) != 0) {
								continue;
							}
							// MATCH,	MATCH
							d.opcode_sz = 1;
							d.opcode[0] = i;
							d.operand_sz = 0;
							d.operand_label[0] = 0;
							break;
						} else if (e.p_op2_reg[0] != 0 && strcmp(e.p_op2_reg, p.operand2.reg) == 0 && e.operand_sz != 0) {
							// LBL_VAL,		MATCH
							if (p.operand1.reg[0] == 0) {
								d.operand_label[0] = 0;
								d.operand_literal = p.operand1.value;
							} else {
								if (label_is_reserved(p.operand1.reg)) continue;
								strncpy(d.operand_label, p.operand1.reg, LABEL_MAX_LEN+1);
							}
							d.opcode_sz = 1;
							d.opcode[0] = i;
							d.operand_sz = e.operand_sz;
							break;
						} else if (e.p_op1_reg[0] != 0 && strcmp(e.p_op1_reg, p.operand1.reg) == 0 && e.operand_sz != 0) {
							if (p.operand2.reg[0] == 0) {
								d.operand_label[0] = 0;
								d.operand_literal = p.operand2.value;
							} else {
								if (label_is_reserved(p.operand2.reg)) continue;
								strncpy(d.operand_label, p.operand2.reg, LABEL_MAX_LEN+1);
							}
							d.opcode_sz = 1;
							d.opcode[0] = i;
							d.operand_sz = e.operand_sz;
							break;

						}
					}
				}
			}
		}
	}

	if (d.opcode_sz == 0) {
		die("unknown instruction");
	}

	return d;

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
	
	data->opcode_sz = 0;
	data->operand_sz = 0;

	switch (opcode_name[0]) {
		case '.':
			// PSEUDO OPS
			if (strcmp(opcode_name + 1, "org") == 0) {

				uint16_t newAddr;
				newAddr = (uint16_t)strtol(operand_name, NULL, 0);

				// -1 means pseudo opcode
				data->opcode_sz = -1;
				data->pseudo_op = PSEUDO_ORG;
				data->operand_sz = 2;
				data->operand_label[0] = '\0';
				data->operand_literal = newAddr;
			}
			break;
		default:
			{	// NORMAL INSTRUCTION
				struct ParsedInstruction instruction;
				instruction = instruction_parse(opcode_name, operand_name);

#ifndef NDEBUG
				if (instruction.operands >= 1)
					fprintf(stderr, "1 { reg=%s, val=%d, isIndirect=%d }\n", instruction.operand1.reg, instruction.operand1.value, instruction.operand1.isIndirect);
				if (instruction.operands == 2)
					fprintf(stderr, "2 { reg=%s, val=%d, isIndirect=%d }\n", instruction.operand2.reg, instruction.operand2.value, instruction.operand2.isIndirect);
#endif

				// now figure out wtf this instruction means
				struct DecodedInstruction decoded;
				decoded = instruction_decode(instruction);

				data->opcode_sz = decoded.opcode_sz;
				data->opcode[0] = decoded.opcode[0];
				data->opcode[1] = decoded.opcode[1];
				data->opcode[2] = decoded.opcode[2];

				data->operand_sz = decoded.operand_sz;
				if (decoded.operand_label[0] == '\0')
					data->operand_label[0] = '\0';
				else
					strncpy(data->operand_label, decoded.operand_label, LABEL_MAX_LEN+1);
				data->operand_literal = decoded.operand_literal;

			}
			break;
		}

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
	case '\'':
	case '\t':
		return true;
	}
	return false;
}

// returns number of bytes used by the line's instruction
size_t parseline(char *line, struct line_data *data, int line_no)
{

	data->new_label = NULL;
	data->opcode_sz = 0;
	data->operand_sz = 0;
	data->operand_label[0] = '\0';

	if (line[0] == ';') return 0;

	int index = 0; // index into line

	// BEGIN LABEL PARSING
	// to define label, loop until TAB/LF/: is encountered
	if (line[0] != '\t' &&
		line[0] != '\n') {
		// there is a label on this line
		
		if ( isdigit(line[0]) != 0 || is_label_char_valid(line[0]) == 0) {
			die("line %d: Label begins with invalid character\n", line_no);
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

					die("line %d: Label contains an invalid character\n", line_no);
	
				} else {
					break;
				}

			}
		}
		// TODO: instead of malloc, return the length of substring in current line, we know it always starts at column zero.
		// This means malloc() is only called during the first pass and not the second

		// 'len' is now the length of the label name (ex. \0)

		if (len > LABEL_MAX_LEN) {
			die("line %d: label name too long\n", line_no);
		}

		data->new_label = malloc(len + 1);
		for (int i = 0; i < len; i++) {
			data->new_label[i] = tolower(line[i]);
		}
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
				die("line %d: Opcode name too long\n", line_no);
			}
		} else {
			// char is not valid or is end of opcode
			if (	line[index] != '\n' &&
				line[index] != '\t' &&
				line[index] != ' '	) {
				die("line %d: Opcode contains invalid characters, char: %2X\n", line_no, line[index]);
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
				die("ERROR: line %d: operand contains invalid character: %2X (hex)\n", line_no, line[index]);
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
		die("on line %d: operand too long\n", line_no);
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

	if (data->opcode_sz == -1) { // pseudo opcode
		return 0;
	}
	return data->opcode_sz + data->operand_sz;

}
