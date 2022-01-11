	.org 0
	ld		a, 0xFF
	out		(0x1), a

stop:
	halt
	jr stop
