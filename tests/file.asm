	.ORG 0xc000
mylabel:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	ld		a, 55
	add		a, 0x0F
	halt
	jr		mylabel