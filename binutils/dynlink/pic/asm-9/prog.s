	.export	_start

	.import	dat1
	.import	dat2
	.import	dat3
	.import	dat4
	.import	dat5

	.code

_start:	.gotadr	$3
	.gotptr	$1,$3,dat3
	ldw	$8,$1,0
	.gotptr	$1,$3,dat4
	ldw	$7,$1,0
	.gotptr	$1,$3,dat3
	ldw	$6,$1,0
	.gotptr	$1,$3,dat4
	ldw	$5,$1,0
loop:	j	loop
