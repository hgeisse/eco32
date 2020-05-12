;
; iasrv.s -- integer arithmetic server
;

	.set	tba,0xF0300000		; terminal base address
	.set	tos,0xC0020000		; top of stack

	; get some addresses listed in the load map
	.export	start
	.export	main
	.export	in
	.export	out

	; minimal execution environment
start:
	add	$29,$0,tos		; setup stack
	jal	main			; do useful work
start1:
	j	start1			; halt by looping

	; main program
main:
	sub	$29,$29,12		; create stack frame
	stw	$31,$29,0		; save return register
	stw	$16,$29,4		; save register variables
	stw	$17,$29,8
main1:
	jal	in			; wait for SYN
	add	$8,$0,'s'
	bne	$2,$8,main1
	add	$4,$0,'a'		; send ACK
	jal	out
main2:
	jal	in			; get operator
	add	$8,$0,1
	beq	$2,$8,op_add
	add	$8,$0,2
	beq	$2,$8,op_sub
	add	$8,$0,3
	beq	$2,$8,op_mulu
	add	$8,$0,4
	beq	$2,$8,op_divu
	add	$8,$0,5
	beq	$2,$8,op_remu
	add	$8,$0,6
	beq	$2,$8,op_muli
	add	$8,$0,7
	beq	$2,$8,op_divi
	add	$8,$0,8
	beq	$2,$8,op_remi
	add	$8,$0,9
	beq	$2,$8,op_eq
	add	$8,$0,10
	beq	$2,$8,op_ne
	add	$8,$0,11
	beq	$2,$8,op_leu
	add	$8,$0,12
	beq	$2,$8,op_ltu
	add	$8,$0,13
	beq	$2,$8,op_geu
	add	$8,$0,14
	beq	$2,$8,op_gtu
	add	$8,$0,15
	beq	$2,$8,op_lei
	add	$8,$0,16
	beq	$2,$8,op_lti
	add	$8,$0,17
	beq	$2,$8,op_gei
	add	$8,$0,18
	beq	$2,$8,op_gti
	j	mainx			; leave in case it's unknown

op_add:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	add	$4,$16,$17
	jal	putword
	j	main2

op_sub:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	sub	$4,$16,$17
	jal	putword
	j	main2

op_mulu:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	mulu	$4,$16,$17
	jal	putword
	j	main2

op_divu:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	divu	$4,$16,$17
	jal	putword
	j	main2

op_remu:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	remu	$4,$16,$17
	jal	putword
	j	main2

op_muli:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	mul	$4,$16,$17
	jal	putword
	j	main2

op_divi:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	div	$4,$16,$17
	jal	putword
	j	main2

op_remi:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	rem	$4,$16,$17
	jal	putword
	j	main2

op_eq:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	add	$4,$0,1
	beq	$16,$17,op_eq1
	add	$4,$0,0
op_eq1:
	jal	putword
	j	main2

op_ne:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	add	$4,$0,1
	bne	$16,$17,op_ne1
	add	$4,$0,0
op_ne1:
	jal	putword
	j	main2

op_leu:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	add	$4,$0,1
	bleu	$16,$17,op_leu1
	add	$4,$0,0
op_leu1:
	jal	putword
	j	main2

op_ltu:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	add	$4,$0,1
	bltu	$16,$17,op_ltu1
	add	$4,$0,0
op_ltu1:
	jal	putword
	j	main2

op_geu:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	add	$4,$0,1
	bgeu	$16,$17,op_geu1
	add	$4,$0,0
op_geu1:
	jal	putword
	j	main2

op_gtu:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	add	$4,$0,1
	bgtu	$16,$17,op_gtu1
	add	$4,$0,0
op_gtu1:
	jal	putword
	j	main2

op_lei:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	add	$4,$0,1
	ble	$16,$17,op_lei1
	add	$4,$0,0
op_lei1:
	jal	putword
	j	main2

op_lti:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	add	$4,$0,1
	blt	$16,$17,op_lti1
	add	$4,$0,0
op_lti1:
	jal	putword
	j	main2

op_gei:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	add	$4,$0,1
	bge	$16,$17,op_gei1
	add	$4,$0,0
op_gei1:
	jal	putword
	j	main2

op_gti:
	jal	getword
	add	$16,$0,$2
	jal	getword
	add	$17,$0,$2
	add	$4,$0,1
	bgt	$16,$17,op_gti1
	add	$4,$0,0
op_gti1:
	jal	putword
	j	main2

mainx:
	ldw	$31,$29,0		; restore return register
	ldw	$16,$29,4		; restore register variables
	ldw	$17,$29,8
	add	$29,$29,12		; release stack frame
	jr	$31			; return

	; input a 4-byte word
getword:
	sub	$29,$29,8
	stw	$31,$29,0
	stw	$16,$29,4
	jal	in
	sll	$16,$16,8
	or	$16,$16,$2
	jal	in
	sll	$16,$16,8
	or	$16,$16,$2
	jal	in
	sll	$16,$16,8
	or	$16,$16,$2
	jal	in
	sll	$16,$16,8
	or	$16,$16,$2
	add	$2,$0,$16
	ldw	$31,$29,0
	ldw	$16,$29,4
	add	$29,$29,8
	jr	$31

	; output a 4-byte word
putword:
	sub	$29,$29,8
	stw	$31,$29,0
	stw	$16,$29,4
	add	$16,$0,$4
	slr	$4,$4,24
	jal	out
	add	$4,$0,$16
	slr	$4,$4,16
	jal	out
	add	$4,$0,$16
	slr	$4,$4,8
	jal	out
	add	$4,$0,$16
	jal	out
	ldw	$31,$29,0
	ldw	$16,$29,4
	add	$29,$29,8
	jr	$31

	; input a character from the serial line
in:
	add	$8,$0,tba		; set I/O base address
in1:
	ldw	$9,$8,0			; get rcvr status
	and	$9,$9,1			; rcvr ready?
	beq	$9,$0,in1		; no - wait
	ldw	$2,$8,4			; get char
	and	$2,$2,0xFF
	jr	$31			; return

	; output a character to the serial line
out:
	add	$8,$0,tba		; set I/O base address
out1:
	ldw	$9,$8,8			; get xmtr status
	and	$9,$9,1			; xmtr ready?
	beq	$9,$0,out1		; no - wait
	and	$4,$4,0xFF
	stw	$4,$8,12		; send char
	jr	$31			; return
