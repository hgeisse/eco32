;
; start.s -- startup code
;

	.set	stacktop,0xC0400000	; at top of 4 MB

	.set	PSW,0			; reg # of PSW
	.set	V_SHIFT,27		; interrupt vector ctrl bit
	.set	V,1 << V_SHIFT

	.export	_start
	.export	_intr
	.export	_refill

	.import	main

	.code

_start:
	j	start

_intr:
	j	_intr			; interrupts better not yet

_refill:
	j	_refill			; user TLB misses better not yet

start:
	.ldgot	$25
	; let irq/exc vectors point to RAM
	add	$8,$0,V
	mvts	$8,PSW
	; setup stack and do some work
	add	$29,$0,stacktop
	jal	main
	; the standalone program's equivalent to exit
stop:
	j	stop
