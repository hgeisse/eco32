;
; keyboard test
;

	.nosyn

	ldhi	$4,0xF0200000
try:	ldw	$5,$4,0
	and	$5,$5,1
	beq	$5,$0,try
	ldw	$5,$4,4
stop:	add	$0,$5,$5
	j	stop
