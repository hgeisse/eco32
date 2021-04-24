	.export	_start
	.import	_end
	.import	main

	.code
_start:
	add	$29,$0,_end + 0x10000
	jal	main
_stop:
	j	_stop
