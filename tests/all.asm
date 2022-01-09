	.org	0
	nop
	ld		bc, 0
	ld		(bc), a
	inc		bc
	inc		b
	dec		b
	ld		b, 0
	rlca
	ex		af, af'
	add		hl, bc
	ld		a, (bc)
	dec		bc
	inc		c
	dec		c
	ld		c, 0
	rrca

	djnz	0
	ld		de,	0
	ld		(de), a
	inc		de
	inc		d
	dec		d
	ld		d, 0
	rla
	jr		0
	add		hl, de
	ld		a, (de)
	dec		de
	inc		e
	dec		e
	ld		e, 0
	rra
	
	jr		nz, 0
	ld		hl,	0
	ld		(0), hl
	inc		hl
	inc		h
	dec		h
	ld		h, 0
	daa
	jr		z, 0
	add		hl,hl
	ld		hl, (0)
	dec		hl
	inc		l
	dec		l
	ld		l, 0
	cpl

	jr		nc, 0
	ld		sp, 0
	ld		(0), a
	inc		sp
	inc		(hl)
	dec		(hl)
	ld		(hl), 0
	scf
	jr		c, 0
	add		hl, sp
	ld		a, (0)
	dec		sp
	inc		a
	dec		a
	ld		a, 0
	ccf

	ld		b, b
	ld		b, c
	ld		b, d
	ld		b, e
	ld		b, h
	ld		b, l
	ld		b, (hl)
	ld		b, a
	ld		c, b
	ld		c, c
	ld		c, d
	ld		c, e
	ld		c, h
	ld		c, l
	ld		c, (hl)
	ld		c, a

	ld		d, b
	ld		d, c
	ld		d, d
	ld		d, e
	ld		d, h
	ld		d, l
	ld		d, (hl)
	ld		d, a
	ld		e, b
	ld		e, c
	ld		e, d
	ld		e, e
	ld		e, h
	ld		e, l
	ld		e, (hl)
	ld		e, a

	ld		h, b
	ld		h, c
	ld		h, d
	ld		h, e
	ld		h, h
	ld		h, l
	ld		h, (hl)
	ld		h, a
	ld		l, b
	ld		l, c
	ld		l, d
	ld		l, e
	ld		l, h
	ld		l, l
	ld		l, (hl)
	ld		l, a
