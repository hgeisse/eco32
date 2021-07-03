;
; start.s -- startup code and assembler support functions
;

	.set	stacktop,0xC0400000	; at top of 4 MB

	.set	PSW,0			; reg # of PSW
	.set	V_SHIFT,27		; interrupt vector ctrl bit
	.set	V,1 << V_SHIFT

	.export	_start
	.export	_intr
	.export	_refill
	.export	run

	.import	main

	.import	getc
	.import	putc
	.import	opendir
	.import	closedir
	.import	readdir
	.import	fopen
	.import	fclose
	.import	fseek
	.import	ftell
	.import	fread

;---------------------------------------------------------------

	.code

_start:
	j	start

_intr:
	j	_intr			; interrupts better not yet

_refill:
	j	_refill			; user TLB misses better not yet

_dummy:
	j	_dummy			; not used

;---------------------------------------------------------------

	.code

;
; system call jump vector
;
	j	halt
	j	getc
	j	putc
	j	opendir
	j	closedir
	j	readdir
	j	fopen
	j	fclose
	j	fseek
	j	ftell
	j	fread

;---------------------------------------------------------------

	.code

;
; startup for standalone program
;
start:
	; let irq/exc vectors point to RAM
	add	$8,$0,V
	mvts	$8,PSW
	; setup stack and do some work
	add	$29,$0,stacktop
	jal	main
	; the standalone program's equivalent to exit
halt:
	j	halt

;---------------------------------------------------------------

	.code

;
; int run(unsigned int startAddr, int argc, char *argv[]);
;
run:
	ccs				; sync caches
	add	$24,$0,$4		; free arg reg
	add	$4,$0,$5		; argc
	add	$5,$0,$6		; argv[]
	jr	$24			; transfer control
