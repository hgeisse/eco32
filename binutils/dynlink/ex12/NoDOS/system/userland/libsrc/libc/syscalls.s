;
; syscalls.s -- system call interface
;

	.set	BASE,0xC0000010

	.set	HALT,BASE + 0 * 4
	.set	GETC,BASE + 1 * 4
	.set	PUTC,BASE + 2 * 4

	.export	halt
	.export	getc
	.export	putc

	.code

;
; void halt(void);
;
halt:
	add	$24,$0,HALT
	jr	$24

;
; char getc(void);
;
getc:
	add	$24,$0,GETC
	jr	$24

;
; void putc(char c);
;
putc:
	add	$24,$0,PUTC
	jr	$24
