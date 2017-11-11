;
; start.s -- startup code
;

	.import	main

	.code

start:
	mvfs	$8,0
	or	$8,$8,1 << 27	; let vector point to RAM
	mvts	$8,0
	add	$29,$0,stack	; set sp
	jal	main		; call 'main'
start1:
	j	start1		; loop

	.bss

	.align	4
	.space	0x800
stack:
