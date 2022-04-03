; math.z80

; div_d_e
; performs integer division d / e
; d - quotient, a - remainder
div_d_e
	xor	a
	ld	b, 8
div_d_e_loop
	rla
	cp	e
	jr	div_d_e_2
	sub	e
	inc	d
div_d_e_2
	djnz	div_d_e_loop
	ret

; quotient goes in ac
; remainder goes in hl
div_ac_de:
	ld	hl, 0
	ld	b, 16
div_ac_de_loop:
	rla
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
cmpgte:
		ld		a, h
		xor d
		jr nc,cmpgte3
cmpgte1	scf             ;false
		ld hl,0
		ret
cmpgte2
		jr cmpgte1
cmpgte3	or a            ;true
		ld hl,1
		ret
