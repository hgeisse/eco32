	.export	_start
	.export	dat3
	.import	dat4

	.code
_start:	.gotadr	$3

	; case 1: static + gotoff
	.gotoff	$8,$3,dat2

	; case 2: static + gotptr
	.gotptr	$8,$3,dat2

	; case 3: export + gotoff
	.gotoff	$8,$3,dat3

	; case 4: export + gotptr
	.gotptr	$8,$3,dat3

	; case 5: import + gotoff
	.gotoff	$8,$3,dat4

	; case 6: import + gotptr
	.gotptr	$8,$3,dat4

loop:	j	loop

	.data
dat1:	.word	0x12345678
dat2:	.word	0x23456789
dat3:	.word	0x3456789A
