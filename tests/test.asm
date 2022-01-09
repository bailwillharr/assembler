	.org	0
	nop
	ld		bc, 0x1234
	ld		(bc), a
	inc		bc
	inc		b
	dec		b
	ld		b, 0xEE
	rlca
	ex		af, af'
	add		hl, bc
	ld		a, (bc)
	dec		bc
	inc		c
	dec		c
	ld		c, 123
	rrca
	djnz	0x00
	ld		de,	0x00
	ld		(de), a
	inc		de
	jr		0x12
	add		hl, de
	ld		a, (de)
	dec		de
	inc		e
	dec		e
	ld		e, 0x77
	add		hl,	bc
	add		hl,	bc
	add		hl,	bc
	add		hl,	bc
	add		hl,	bc
	add		hl,	bc
	add		hl,	bc
	add		hl,	bc
	add		hl,	bc
	add		hl,	bc
	add		hl,	bc
	add		hl,	bc
	add		hl,	bc
	add		hl,	bc
	add		hl,	bc
	add		hl,	bc
	add		hl,	bc
	add		hl,	bc
