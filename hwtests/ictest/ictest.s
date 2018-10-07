;
; ictest.s -- test instruction cache
;

	.locate	0x0000

part1:	add	$8,$0,16
l1:	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	sub	$8,$8,1
	bne	$8,$0,l1
	j	part2

	.locate	0x1000

part3:	add	$8,$0,64
l3:	j	l4

	.locate	0x1300

l5:	j	l6

	.locate	0x2000

part4:	add	$8,$0,8
l7:	j	l8
l9:	j	l10
l11:	j	l12
l13:	j	l14
l15:	j	l16
l17:	j	l18
l19:	j	l20
l21:	j	l22
l23:	j	l24
l25:	j	l26
l27:	j	l28
l29:	j	l30
l31:	j	l32
l33:	j	l34

	.locate	0x4000

part2:	add	$8,$0,16
l2:	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	add	$1,$2,$3
	sub	$8,$8,1
	bne	$8,$0,l2
	j	part3

	.locate	0x5000

l4:	j	l5

	.locate	0x5300

l6:	sub	$8,$8,1
	bne	$8,$0,l3
	j	part4

	.locate	0x6000

l8:	j	l9
l10:	j	l11
l12:	j	l13
l14:	j	l15
l16:	j	l17
l18:	j	l19
l20:	j	l21
l22:	j	l23
l24:	j	l25
l26:	j	l27
l28:	j	l29
l30:	j	l31
l32:	j	l33
l34:	sub	$8,$8,1
	bne	$8,$0,l7
halt:	j	halt
