	.export	_start

	.code
	.nosyn

_start:	jal	getpc
getpc:	ldhi	$3,0x0000	; HI(got-getpc)
	or	$3,$3,0x0FFC	; LO(got-getpc)
	add	$3,$3,$31
	ldw	$1,$3,3*4	; load GOT pointer 3
	ldw	$8,$1,0
loop:	j	loop

	.data

	; access to global data uses a pointer from the GOT
got:	.word	0
	.word	0
	.word	0
	.word	dat3
	.word	0

	; the data may live in another library and thus
	; its address may not be known until run-time
dat1:	.word	0x12345678
dat2:	.word	0x23456789
dat3:	.word	0x3456789A
dat4:	.word	0x456789AB
dat5:	.word	0x56789ABC
