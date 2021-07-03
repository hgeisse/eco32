;
; syscalls.s -- system call interface
;

	.set	BASE,0xC0000010

	.set	HALT,BASE + 0 * 4
	.set	GETCHAR,BASE + 1 * 4
	.set	PUTCHAR,BASE + 2 * 4
	.set	OPENDIR,BASE + 3 * 4
	.set	CLOSEDIR,BASE + 4 * 4
	.set	READDIR,BASE + 5 * 4
	.set	FOPEN,BASE + 6 * 4
	.set	FCLOSE,BASE + 7 * 4
	.set	FSEEK,BASE + 8 * 4
	.set	FTELL,BASE + 9 * 4
	.set	FREAD,BASE + 10 * 4

	.export	_halt
	.export	_getchar
	.export	_putchar
	.export	_opendir
	.export	_closedir
	.export	_readdir
	.export	_fopen
	.export	_fclose
	.export	_fseek
	.export	_ftell
	.export	_fread

	.code

;
; void _halt(void);
;
_halt:
	add	$24,$0,HALT
	jr	$24

;
; char _getchar(void);
;
_getchar:
	add	$24,$0,GETCHAR
	jr	$24

;
; void _putchar(char c);
;
_putchar:
	add	$24,$0,PUTCHAR
	jr	$24

;
; DIR *_opendir(void);
;
_opendir:
	add	$24,$0,OPENDIR
	jr	$24

;
; void _closedir(DIR *dp);
;
_closedir:
	add	$24,$0,CLOSEDIR
	jr	$24

;
; int _readdir(DirEntry *buf, DIR *dp);
;
_readdir:
	add	$24,$0,READDIR
	jr	$24

;
; FILE *_fopen(char *path);
;
_fopen:
	add	$24,$0,FOPEN
	jr	$24

;
; void _fclose(FILE *fp);
;
_fclose:
	add	$24,$0,FCLOSE
	jr	$24

;
; int _fseek(FILE *fp, int offset, int whence);
;
_fseek:
	add	$24,$0,FSEEK
	jr	$24

;
; int _ftell(FILE *fp);
;
_ftell:
	add	$24,$0,FTELL
	jr	$24

;
; int _fread(void *buf, int size, int num, FILE *fp);
;
_fread:
	add	$24,$0,FREAD
	jr	$24
