#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <inttypes.h>

// Both real and pseudo instructions

// does not include null terminator
#define OPCODE_NAME_MAX_LEN 4

enum pseudo_opcode { PSEUDO_DB, PSEUDO_DW, PSEUDO_ORG };

enum OpcodesMain {
/*	0			1			2			3			4			5			6			7
 *	8			9			A			B			C			D			E			F		*/
	NOP,		LD_BC_NN,	LD_xBC_A,	INC_BC,		INC_B,		DEC_B,		LD_B_N,		RLCA,
	EX_AF_AF,	ADD_HL_BC,	LD_A_xBC,	DEC_BC,		INC_C,		DEC_C,		LD_C_N,		RRCA,

	DJNZ_N,		LD_DE_NN,	LD_xDE_A,	INC_DE,		INC_D,		DEC_D,		LD_D_N,		RLA,
	JR_N,		ADD_HL_DE,	LD_A_xDE,	DEC_DE,		INC_E,		DEC_E,		LD_E_N,		RRA,

	JR_NZ_N,	LD_HL_NN,	LD_xNN,HL,	INC_HL,		INC_H,		DEC_H,		LD_H,N,		DAA,
	JR_Z_N,		ADD_HL_HL,	LD_HL_xNN,	DEC_HL,		INC_L,		DEC_L,		LD_L_N,		CPL,

	JR_NC_N,	LD_SP_NN,	LD_xNN_A,	INC_SP,		INC_xHL,	DEC_xHL,	LD_xHL_N,	SCF,
	JR_C_N,		ADD_HL_SP,	LD_A_xNN,	DEC_SP,		INC_A,		DEC_A,		LD_A_N,		CCF,

	LD_B_B,		LD_B_C,		LD_B_D,		LD_B_E,		LD_B_H,		LD_B_L,		LD_B_xHL,	LD_B_A,
	LD_C_B,		LD_C_C,		LD_C_D,		LD_C_E,		LD_C_H,		LD_C_L,		LD_C_xHL,	LD_C_A,

	LD_D_B,		LD_D_C,		LD_D_D,		LD_D_E,		LD_D_H,		LD_D_L,		LD_D_xHL,	LD_D_A,
	LD_E_B,		LD_E_C,		LD_E_D,		LD_E_E,		LD_E_H,		LD_E_L,		LD_E_xHL,	LD_E_A,

	LD_H_B,		LD_H_C,		LD_H_D,		LD_H_E,		LD_H_H,		LD_H_L,		LD_H_xHL,	LD_H_A,
	LD_L_B,		LD_L_C,		LD_L_D,		LD_L_E,		LD_L_H,		LD_L_L,		LD_L_xHL,	LD_L_A,

	LD_xHL,B,	LD_xHL_C,	LD_xHL_D,	LD_xHL_E,	LD_xHL_H,	LD_xHL_L,	HALT,		LD_xHL_A,
	LD_A_B,		LD_A_C,		LD_A_D,		LD_A_E,		LD_A_H,		LD_A_L,		LD_A_xHL,	LD_A_A,

	ADD_A_B,	ADD_A_C,	ADD_A_D,	ADD_A_E,	ADD_A_H,	ADD_A_L,	ADD_A_xHL,	ADD_A_A,
	ADC_A_B,	ADC_A_C,	ADC_A_D,	ADC_A_E,	ADC_A_H,	ADC_A_L,	ADC_A_xHL,	ADC_A_A,

	SUB_B,		SUB_C,		SUB_D,		SUB_E,		SUB_H,		SUB_L,		SUB_xHL,	SUB_A,
	SBC_A_B,	SBC_A_C,	SBC_A_D,	SBC_A_E,	SBC_A_H,	SBC_A_L,	SBC_A_xHL,	SBC_A_A,

	AND_B,		AND_C,		AND_D,		AND_E,		AND_H,		AND_L,		AND_xHL,	AND_A,
	XOR_B,		XOR_C,		XOR_D,		XOR_E,		XOR_H,		XOR_L,		XOR_xHL,	XOR_A,

	OR_B,		OR_C,		OR_D,		OR_E,		OR_H,		OR_L,		OR_xHL,		OR_A,
	CP_B,		CP_C,		CP_D,		CP_E,		CP_H,		CP_L,		CP_xHL,		CP_A,

	RET_NZ,		POP_BC,		JP_NZ_NN,	JP_NN,		CALL_NZ_NN,	PUSH_BC,	ADD_A_N,	RST_00,
	RET_Z,		RET,		JP_Z_NN,	PRE_BITS,	CALL_Z_NN,	CALL_NN,	ADC_A_N,	RST_08,

	RET_NC,		POP_DE,		JP_NC_NN,	OUT_xN_A,	CALL_NC_NN,	PUSH_DE,	SUB_N,		RST_10,
	RET_C,		EXX,		JP_C_NN,	IN_A_xN,	CALL_C_NN,	PRE_IX,		SBC_A_N,	RST_18,

};

#endif
