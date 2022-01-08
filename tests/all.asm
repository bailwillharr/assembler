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
	djnz	END
	ld		de,	END
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
END:
