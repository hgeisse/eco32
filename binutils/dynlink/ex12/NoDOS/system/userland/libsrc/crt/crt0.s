;
; crt0.s -- startup code
;

	.export	_start

	.import	main

	.code

_start:
	j	main