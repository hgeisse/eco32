;
; c0.s -- startup code
;

	.import	_bcode
	.import	_ecode
	.import	_bdata
	.import	_edata
	.import	_bbss
	.import	_ebss

	.import	main

	.export	_start

	.code
	.align	4

_start:
	jal	main		; call 'main' function
_stop:
	j	_stop		; just to be sure...
