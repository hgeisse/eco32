	.export	_start
	.import	main

	.code
_start:
	add	$29,$0,0xC0010000
	jal	main
_stop:
	j	_stop
