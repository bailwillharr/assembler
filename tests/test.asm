	.ORG $8000
label_test
	inc bc
	halt
	jp		label_test
	nop
	jp		(hl)
