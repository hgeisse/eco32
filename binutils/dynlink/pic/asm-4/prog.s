	.export	_start

	.code

_start:	.ldgot	$3
	ldw	$8,$0,dat3
loop:	j	loop

	.data

dat1:	.word	0x12345678
dat2:	.word	0x23456789
dat3:	.word	0x3456789A
dat4:	.word	0x456789AB
dat5:	.word	0x56789ABC
