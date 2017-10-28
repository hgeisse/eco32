;
; test_01.s -- different (defined) symbols, no relocs
;

	.set	lbl0,0x55AA

	.code
	add	$4,$2,$3
	add	$5,$2,$3
lbl1:
	add	$6,$2,$3
	add	$7,$2,$3

	.data
	.word	0x1234
lbl2:
	.word	0x5678

	.bss
	.space	0x200
lbl3:
	.space	0x100

	.export	lbl0
	.export	lbl1
	.export	lbl2
	.export	lbl3
