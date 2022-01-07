; testing opcodes
	nop
TEST_LABEL:
	ld		(bc), a
	ld		bc,		 TEST_LABEL
