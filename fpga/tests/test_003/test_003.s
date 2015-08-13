;
; simple instruction test
;

	.nosyn

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	sub	$3,$4,0x8765

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	add	$0,$3,$3	; $3 = 0x0E7DE5CD

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
