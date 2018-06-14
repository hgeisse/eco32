	.export	_start

	.code
_start:	.gota	$1
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
