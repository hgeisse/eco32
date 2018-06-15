	.export	_start
	.export	dat2
	.export	dat3

	.code
_start:	.gotadr	$3
	.gotptr	$8,$3,dat3
	ldw	$8,$8,0
	.gotoff	$9,$3,dat4
	ldw	$9,$9,0
loop:	j	loop

	.data
dat1:	.word	0x12345678
dat2:	.word	0x23456789
dat3:	.word	0x3456789A
dat4:	.word	0x456789AB
dat5:	.word	0x56789ABC
