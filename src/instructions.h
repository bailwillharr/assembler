#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <inttypes.h>

// Both real and pseudo instructions

// does not include null terminator
#define OPCODE_NAME_MAX_LEN 4

enum pseudo_opcode { PSEUDO_DB, PSEUDO_DW, PSEUDO_ORG };

enum OpcodesMain {
/*	0			1			2			3			4			5			6			7			8			9			A			B			C			D			E			F			*/
	NOP,		LD_BC_NN,	LD_xBC_A,	INC_BC,		INC_B,		DEC_B,		LD_B_N,		RLCA,		EX_AF_AF,	ADD_HL_BC,	LD_A_xBC,	DEC_BC,		INC_C,		DEC_C,		LD_C_N,		RRCA,
};

#endif
