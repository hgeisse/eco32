;
; simple instruction test
;

	.nosyn

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	and	$3,$4,0x8765

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0

	add	$0,$3,$3	; $3 = 0x00000520

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
