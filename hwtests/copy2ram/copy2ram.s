;
; copy2ram.s -- copy a tiny program to RAM and execute it there
;

; $4 current source address
; $5 source end address
; $6 current target address
; $7 word to copy

; $7 character to output
; $8 I/O base
; $9 I/O status

	.set	ram_base,0xC0000000
	.set	io_base,0xF0300000

copy:
	add	$4,$0,start
	add	$5,$0,end
	add	$6,$0,ram_base
loop:
	ldw	$7,$4,0
	stw	$7,$6,0
	add	$4,$4,4
	add	$6,$6,4
	bne	$4,$5,loop
	cctl	3			; flush data cache
	add	$7,$0,ram_base		; jump to program
	jr	$7

	;
	; this program gets copied to ram_base
	; all jumps, calls and branches are relative to the
	; current PC, so it can run at ram_base although it
	; was bound for a different location
	;
start:
	add	$7,$0,'.'
	jal	out
halt:
	j	halt

	;
	; output on terminal
	;
out:
	add	$8,$0,io_base
out1:
	ldw	$9,$8,8
	and	$9,$9,1
	beq	$9,$0,out1
	stw	$7,$8,12
	jr	$31
end:
