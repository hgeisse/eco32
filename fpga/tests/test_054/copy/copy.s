;
; copy.s -- copy a program from ROM to RAM before executing it
;

	.set	dst,0xC0000000		; destination is start of RAM
	.import	size			; number of bytes to be copied

	.set	PSW,0			; reg # of PSW

	.nosyn

reset:
	j	start

interrupt:
	j	interrupt		; we better have no interrupts

userMiss:
	j	userMiss		; and no user TLB misses

start:
	mvts	$0,PSW			; disable interrupts and user mode

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	ldhi	$8,src

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	or	$8,$8,src

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	ldhi	$9,dst

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	add	$10,$9,size

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

loop:
	ldw	$11,$8,0		; copy word

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	stw	$11,$9,0

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	add	$8,$8,4			; bump pointers

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	add	$9,$9,4

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	bltu	$9,$10,loop		; more?

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	ldhi	$8,dst			; start execution

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	jr	$8

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	; the program to be copied follows immediately
src:
