;
; ictest.s -- test instruction cache
;

part1:
	add	$8,$0,16
l1:
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
	add	$1,$2,$3
	sub	$8,$8,1
	bne	$8,$0,l1
	j	part2

	.locate	0x2000

part3:
	add	$8,$0,64
l3:
	j	l4

	.locate	0x2300

l5:
	j	l6

	.locate	0x4000

part2:
	add	$8,$0,16
l2:
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
	add	$1,$2,$3
	sub	$8,$8,1
	bne	$8,$0,l2
	j	part3

	.locate	0x6000

l4:
	j	l5

	.locate	0x6300

l6:
	sub	$8,$8,1
	bne	$8,$0,l3
halt:
	j	halt
