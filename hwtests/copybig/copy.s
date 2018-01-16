;
; copy.s -- copy data from ROM to RAM and compare
;

	.set	dst,0xC0000000		; destination is start of RAM
	.set	len,0x0000FF00		; number of bytes to be copied

	.set	PSW,0			; reg # of PSW

reset:
	j	start

interrupt:
	j	interrupt		; we better have no interrupts

userMiss:
	j	userMiss		; and no user TLB misses

start:
	mvts	$0,PSW			; disable interrupts and user mode
	; copy
	add	$8,$0,src
	add	$9,$0,dst
	add	$10,$9,len
loop:
	ldw	$11,$8,0		; copy word
	stw	$11,$9,0
	add	$8,$8,4			; bump pointers
	add	$9,$9,4
	bltu	$9,$10,loop		; more?
	cctl	3			; flush dcache
	; compare
	add	$8,$0,src
	add	$9,$0,dst
	add	$10,$9,len
loop2:
	ldw	$11,$8,0		; compare word
	ldw	$12,$9,0
	bne	$11,$12,fail
	add	$8,$8,4			; bump pointers
	add	$9,$9,4
	bltu	$9,$10,loop2		; more?
	add	$7,$0,'.'
	j	out
fail:
	add	$7,$0,'?'
	; output result
out:
	add	$8,$0,0xF0300000
out1:
	ldw	$9,$8,8
	and	$9,$9,1
	beq	$9,$0,out1
	stw	$7,$8,12
	; halt
stop:
	j	stop

	; the data to be copied follows immediately
src:
