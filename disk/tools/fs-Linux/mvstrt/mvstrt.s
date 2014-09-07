;
; mvstrt.s -- move a program down to 0xC0000000 and start it
;

	.set	dst,0xC0000000		; destination is start of RAM
	.set	len,0x00400000-256	; number of bytes to be copied

start:
	add	$8,$0,src
	add	$9,$0,dst
	add	$10,$9,len
loop:
	ldw	$11,$8,0		; copy word
	stw	$11,$9,0
	add	$8,$8,4			; bump pointers
	add	$9,$9,4
	bltu	$9,$10,loop		; more?
	add	$8,$0,dst		; start execution
	jr	$8

	; source follows immediately
src:
