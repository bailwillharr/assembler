; most variations of Z80 opcodes
	nop
TEST_LABEL:
	ld		(bc), a
	inc		bc
	inc		b
	dec		b
	ld		b, 0x69
	rlca
	ex		af,af'
	jp		TEST_LABEL
	ld		c, 0x99
	halt
	jp		(hl)
	and		(hl)
	rst		0x38
	ex		(sp), hl
	in		a, (0x08)
