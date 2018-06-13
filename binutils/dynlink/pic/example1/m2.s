	.export	_start

	.code
	.nosyn
_start:	jal	getpc
getpc:	ldhi	$1,0x0000	; HI(got-getpc)
	or	$1,$1,0x0FFC	; LO(got-getpc)
	add	$1,$1,$31
	ldw	$8,$1,2*4
	ldw	$8,$8,0
loop:	j	loop

	.data
got:	.word	0
	.word	0
	.word	dat3
	.word	0
	.word	0
dat1:	.word	0x12345678
dat2:	.word	0x23456789
dat3:	.word	0x3456789A
dat4:	.word	0x456789AB
dat5:	.word	0x56789ABC
