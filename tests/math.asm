; math.z80

; div_d_e
; performs integer division d / e
; d - quotient, a - remainder
div_d_e
	xor	a
	ld	b, 8
div_d_e_loop
	sla	d
	rla
	cp	e
;	jr	c, $+4
	jr	div_d_e_2
	sub	e
	inc	d
div_d_e_2
	djnz	div_d_e_loop
	ret

	jp nc, (ix-0X55)

; quotient goes in ac
; remainder goes in hl
div_ac_de:
	ld	hl, 0
	ld	b, 16
div_ac_de_loop:
	sll	c
	rla
	adc	hl, hl
	sbc	hl, de
;	jr	nc, $+4
	jr	div_ac_de_2
	add	hl, de
	dec	c
div_ac_de_2
	djnz	div_ac_de_loop
	ret

;### CMPGTE -> test if A>=B
;### Input      HL=A, DE=B
;### Output     HL=1, CF=0 -> true
;###            HL=0, CF=1 -> false
cmpgte
		ld a,h
		xor d
		jp m,cmpgte2
		sbc hl,de
;		jr nc,cmpgte3
		jr cmpgte3
cmpgte1	scf             ;false
		ld hl,0
		ret
cmpgte2	bit 7,d
		jr cmpgte1
cmpgte3	or a            ;true
		ld hl,1
		ret
