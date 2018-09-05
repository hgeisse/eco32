	.export	func

	.import	dat1
	.import	dat2
	.import	dat3
	.import	dat4
	.import	dat5

	.code

func:	.ldgot	$3
	ldw	$8,$0,dat3
	ldw	$7,$0,dat4
	ldw	$6,$0,dat3
	ldw	$5,$0,dat4
loop:	j	loop
