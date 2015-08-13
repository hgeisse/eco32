;
; loop
;

	.nosyn

reset:
	j	start

interrupt:
	j	interrupt

userMiss:
	j	userMiss

start:
	j	start

	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
	add	$0,$0,$0
