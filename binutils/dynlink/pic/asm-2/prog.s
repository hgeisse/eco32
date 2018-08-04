	.export	_start

	.code
	.nosyn

_start:	jal	getpc
getpc:	ldhi	$3,0x0000	; HI(got-getpc)
	or	$3,$3,0x0FFC	; LO(got-getpc)
	add	$3,$3,$31
	ldhi	$1,0x0000	; HI(dat3-got)
	or	$1,$1,0x001C	; LO(dat3-got)
	add	$1,$1,$3
	ldw	$8,$1,0
loop:	j	loop

	.data

	; access to static data does not use any pointer from the GOT
	; only its position in the virtual address space is needed
got:	.word	0
	.word	0
	.word	0
	.word	0
	.word	0

dat1:	.word	0x12345678
dat2:	.word	0x23456789
dat3:	.word	0x3456789A
dat4:	.word	0x456789AB
dat5:	.word	0x56789ABC
