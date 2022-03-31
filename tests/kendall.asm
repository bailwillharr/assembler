	.org 23298 
	LD BC, (23296)
	CALL GetScreenPos
Line:
	CALL FillLine
	CALL GetNextLine
	LD A, (HL)
	OR A
	JR Z, Line
	RET
FillLine:
	PUSH HL
	CALL FillLeft
	POP HL
	PUSH HL
	INC HL
	CALL FillRight
	POP HL
	RET
FillLeft:
	LD A, (HL)
	OR A
	JR NZ, MaskLeft
	LD (HL), 255
	DEC HL
	JR FillLeft
MaskLeft:
	LD B, 0
	PUSH AF
LeftAgain:
	BIT 0, A
	JR NZ, EndFillLeft
	SLA B
	SET 0, B
	SRA A
	JR LeftAgain
EndFillLeft:
	POP AF
	OR B
	LD (HL), A
	RET
FillRight:
	LD A, (HL)
	OR A
	JR NZ, MaskRight
	LD (HL), 255
	INC HL
	JR FillRight
MaskRight:
	LD B, 0
	PUSH AF
RightAgain:
	BIT 7, A
	JR NZ, EndFillRight
	SRA B
	SET 7, B
	SLA A
	JR RightAgain
EndFillRight:
	POP AF
	OR B
	LD (HL), A
	RET
GetScreenPos:
	LD A, C
	AND 0x38 ; %00111000
	RLCA
	RLCA
	OR B
	LD L, A
	LD A, C
	AND 0x07 ; %00000111
	LD H, A
	LD A, C
	AND 0xC0 ; %11000000
	RRCA
	RRCA
	RRCA
	OR H
	OR 0x40 ; &40
	LD H, A
	RET
; Takes HL as Screen Address
GetNextLine:
	INC H
	LD A, H
	AND 7
	RET NZ
	LD A, L
	ADD A, 32
	LD L, A
	RET C
	LD A, H
	SUB 8
	LD H, A
	RET
