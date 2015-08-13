;
; output to display
;

	.set	dspbase,0xF0100000

	.nosyn

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	ldhi	$8,data

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	or	$8,$8,data

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	ldhi	$9,dspbase

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

next:
	ldw	$10,$8,0

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	beq	$10,$0,stop

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	stw	$10,$9,(14*128+32)*4

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	add	$8,$8,4

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

	j	next

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

stop:
	j	stop

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

data:
	.word	0x00009420
	.word	0x00009448
	.word	0x00009465
	.word	0x0000946C
	.word	0x0000946C
	.word	0x0000946F
	.word	0x0000942C
	.word	0x00009420
	.word	0x00009477
	.word	0x0000946F
	.word	0x00009472
	.word	0x0000946C
	.word	0x00009464
	.word	0x00009421
	.word	0x00009420
	.word	0x00000000
