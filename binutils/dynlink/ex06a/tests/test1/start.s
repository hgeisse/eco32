	.export	_start
	.export	g
	.export	n

	.import	main

	.code
_start:
	jal main
_stop:
	j	_stop

	.code
g:
	jr	$31

	.data
n:
	.word	0x12345678
