; testing opcodes
	ld		hl, 0xBEEF
	ld		(TEST_LABEL), hl
	halt
TEST_LABEL:
	nop
	nop
