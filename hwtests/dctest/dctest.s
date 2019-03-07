;
; dctest.s -- test data cache
;

	add	$8,$0,16
l1:	add	$9,$0,0x200
	add	$10,$0,0xC0000000
	add	$11,$0,0xC0010000
l2:	ldw	$12,$10,0
	stw	$12,$11,0
	add	$10,$10,4
	add	$11,$11,4
	sub	$9,$9,1
	bne	$9,$0,l2
	sub	$8,$8,1
	bne	$8,$0,l1
	dcf
halt:	j	halt
