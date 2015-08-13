;
; keyboard test
;

	.nosyn

	ldhi	$4,0xF0200000
	ldhi	$6,0xF0100000
try:	ldw	$5,$4,0
	and	$5,$5,1
	beq	$5,$0,try
	ldw	$5,$4,4
	or	$5,$5,0x0700
	stw	$5,$6,0
	add	$6,$6,4
	j	try
