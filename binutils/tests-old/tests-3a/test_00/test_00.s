;
; test_00.s -- 3 segments, no symbols, no relocs
;

	.code
	add	$4,$2,$3
	add	$5,$2,$3
	add	$6,$2,$3
	add	$7,$2,$3

	.data
	.word	0x1234
	.word	0x5678

	.bss
	.space	0x300
