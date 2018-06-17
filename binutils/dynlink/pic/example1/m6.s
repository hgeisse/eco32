	.export	_start
	.export	dat3
	.export	dat4

	.code
_start:	.gotadr	$3
	.gotoff	$8,$3,dat2
	ldw	$8,$8,0
	.gotptr	$9,$3,dat3
	ldw	$9,$9,0
loop:	j	loop

	.data
dat1:	.word	0x12345678
dat2:	.word	0x23456789
dat3:	.word	0x3456789A
dat4:	.word	0x456789AB
dat5:	.word	0x56789ABC
