	.export	_start

	.import	dat1
	.import	dat2
	.import	dat3
	.import	dat4
	.import	dat5

	.code

_start:	.ldgot	$3
	ldw	$8,$0,dat3
	ldw	$9,$0,dat4
	ldw	$10,$0,dat3
	ldw	$11,$0,dat4
loop:	j	loop
