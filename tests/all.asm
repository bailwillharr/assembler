	.org	0
	nop
	ld		bc, END
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
	inc		d
	dec		d
	ld		d, 0x99
	rla
	jr		END
	add		hl, de
	ld		a, (de)
	dec		de
	inc		e
	dec		e
	ld		e, 0x77
	rra
	
	jr		nz, END
	ld		hl,	0x1234
	ld		(0xF000), hl
	inc		hl
	inc		h
	dec		h
	ld		h, 0x21
	daa
	jr		z, END
	add		hl,hl
	ld		hl, (0x9999)
	dec		hl
	inc		l
	dec		l
	ld		l, 45
	cpl

	jr		nc, END
	ld		sp, 0x7777
	ld		(0x1234), a
	inc		sp
	inc		(hl)
	dec		(hl)
	ld		(hl), 75
	scf
	jr		c, 99
	add		hl, sp
	ld		a, (4000)
	dec		sp
	inc		a
	dec		a
	ld		a, 38
	ccf
END:
