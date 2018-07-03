%{

/*
 * eco32n.md -- new ECO32 back-end specification
 *
 * register usage:
 *   $0   always zero
 *   $1   reserved for assembler
 *   $2   func return value
 *   $3   func return value
 *   $4   proc/func argument
 *   $5   proc/func argument
 *   $6   proc/func argument
 *   $7   proc/func argument
 *   $8   temporary register (caller-save)
 *   $9   temporary register (caller-save)
 *   $10  temporary register (caller-save)
 *   $11  temporary register (caller-save)
 *   $12  temporary register (caller-save)
 *   $13  temporary register (caller-save)
 *   $14  temporary register (caller-save)
 *   $15  temporary register (caller-save)
 *   $16  register variable  (callee-save)
 *   $17  register variable  (callee-save)
 *   $18  register variable  (callee-save)
 *   $19  register variable  (callee-save)
 *   $20  register variable  (callee-save)
 *   $21  register variable  (callee-save)
 *   $22  register variable  (callee-save)
 *   $23  register variable  (callee-save)
 *   $24  temporary register (caller-save)
 *   $25  temporary register (caller-save)
 *   $26  reserved for OS kernel
 *   $27  reserved for OS kernel
 *   $28  reserved for OS kernel
 *   $29  stack pointer
 *   $30  interrupt return address
 *   $31  proc/func return address
 * caller-save registers are not preserved across procedure calls
 *   (i.e., it's the caller's duty to save/restore them, if their
 *   contents should survive the call)
 * callee-save registers are preserved across procedure calls
 *   (i.e., it's the callee's duty to save/restore them, if one
 *   of them is overwritten by the callee)
 *
 * tree grammar terminals have been produced by:
 *   ops c=1 s=2 i=4 l=4 h=4 f=4 d=8 x=8 p=4
 */

#include "c.h"

#define NODEPTR_TYPE	Node

static void address(Symbol, Symbol, long);
static void defaddress(Symbol);
static void defconst(int, int, Value);
static void defstring(int, char *);
static void defsymbol(Symbol);
static void export(Symbol);
static void function(Symbol, Symbol [], Symbol [], int);
static void global(Symbol);
static void import(Symbol);
static void local(Symbol);
static void progbeg(int, char *[]);
static void progend(void);
static void segment(int);
static void space(int);
static Symbol rmap(int);
static void blkfetch(int, int, int, int);
static void blkstore(int, int, int, int);
static void blkloop(int, int, int, int, int, int []);
static void emit2(Node);
static void doarg(Node);
static void target(Node);
static void clobber(Node);
static void _label(Node);

#define INTTMP	0x0100FF00
#define INTVAR	0x00FF0000
#define INTRET	0x00000004
#define FLTTMP	0x000F0FF0
#define FLTVAR	0xFFF00000
#define FLTRET	0x00000003

static Symbol ireg[32];
static Symbol iregw;
static Symbol freg2[32];
static Symbol freg2w;

static int pic = 0;

%}

%term CNSTF4=4113 CNSTF8=8209
%term CNSTI1=1045 CNSTI2=2069 CNSTI4=4117
%term CNSTP4=4119
%term CNSTU1=1046 CNSTU2=2070 CNSTU4=4118

%term ARGB=41
%term ARGF4=4129 ARGF8=8225
%term ARGI4=4133
%term ARGP4=4135
%term ARGU4=4134

%term ASGNB=57
%term ASGNF4=4145 ASGNF8=8241
%term ASGNI1=1077 ASGNI2=2101 ASGNI4=4149
%term ASGNP4=4151
%term ASGNU1=1078 ASGNU2=2102 ASGNU4=4150

%term INDIRB=73
%term INDIRF4=4161 INDIRF8=8257
%term INDIRI1=1093 INDIRI2=2117 INDIRI4=4165
%term INDIRP4=4167
%term INDIRU1=1094 INDIRU2=2118 INDIRU4=4166

%term CVFF4=4209 CVFF8=8305
%term CVFI4=4213

%term CVIF4=4225 CVIF8=8321
%term CVII1=1157 CVII2=2181 CVII4=4229
%term CVIU1=1158 CVIU2=2182 CVIU4=4230

%term CVPU4=4246

%term CVUI1=1205 CVUI2=2229 CVUI4=4277
%term CVUP4=4279
%term CVUU1=1206 CVUU2=2230 CVUU4=4278

%term NEGF4=4289 NEGF8=8385
%term NEGI4=4293

%term CALLB=217
%term CALLF4=4305 CALLF8=8401
%term CALLI4=4309
%term CALLP4=4311
%term CALLU4=4310
%term CALLV=216

%term RETF4=4337 RETF8=8433
%term RETI4=4341
%term RETP4=4343
%term RETU4=4342
%term RETV=248

%term ADDRGP4=4359

%term ADDRFP4=4375

%term ADDRLP4=4391

%term ADDF4=4401 ADDF8=8497
%term ADDI4=4405
%term ADDP4=4407
%term ADDU4=4406

%term SUBF4=4417 SUBF8=8513
%term SUBI4=4421
%term SUBP4=4423
%term SUBU4=4422

%term LSHI4=4437
%term LSHU4=4438

%term MODI4=4453
%term MODU4=4454

%term RSHI4=4469
%term RSHU4=4470

%term BANDI4=4485
%term BANDU4=4486

%term BCOMI4=4501
%term BCOMU4=4502

%term BORI4=4517
%term BORU4=4518

%term BXORI4=4533
%term BXORU4=4534

%term DIVF4=4545 DIVF8=8641
%term DIVI4=4549
%term DIVU4=4550

%term MULF4=4561 MULF8=8657
%term MULI4=4565
%term MULU4=4566

%term EQF4=4577 EQF8=8673
%term EQI4=4581
%term EQU4=4582

%term GEF4=4593 GEF8=8689
%term GEI4=4597
%term GEU4=4598

%term GTF4=4609 GTF8=8705
%term GTI4=4613
%term GTU4=4614

%term LEF4=4625 LEF8=8721
%term LEI4=4629
%term LEU4=4630

%term LTF4=4641 LTF8=8737
%term LTI4=4645
%term LTU4=4646

%term NEF4=4657 NEF8=8753
%term NEI4=4661
%term NEU4=4662

%term JUMPV=584

%term LABELV=600


%%


%%


static void address(Symbol q, Symbol p, long n) {
  print("; *** address(%s, %s, %ld)\n", q->name, p->name, n);
}


static void defaddress(Symbol p) {
  print("; *** defaddress(%s)\n", p->name);
}


static void defconst(int suffix, int size, Value v) {
  print("; *** defconst(%d, %d, ...)\n", suffix, size);
}


static void defstring(int len, char *s) {
  print("; *** defstring(%d, %s)\n", len, s);
}


static void defsymbol(Symbol p) {
  print("; *** defsymbol(%s)\n", p->name);
}


static void export(Symbol p) {
  print("; *** export(%s)\n", p->name);
}


static void function(Symbol f, Symbol caller[],
                     Symbol callee[], int ncalls) {
  print("; *** function(%s, ..., %d)\n", f->name, ncalls);
}


static void global(Symbol p) {
  print("; *** global(%s)\n", p->name);
}


static void import(Symbol p) {
  print("; *** import(%s)\n", p->name);
}


static void local(Symbol p) {
  print("; *** local(%s)\n", p->name);
}


/*
 * record whether the endianness of host and target is different
 */
static void setSwap(void) {
  union {
    char c;
    int i;
  } u;

  u.i = 0;
  u.c = 1;
  swap = ((u.i == 1) != IR->little_endian);
}


static void progbeg(int argc, char *argv[]) {
  int i;

  setSwap();
  segment(CODE);
  parseflags(argc, argv);
  for (i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-pic") == 0) {
      pic = 1;
    }
  }
  for (i = 0; i < 32; i++) {
    ireg[i] = mkreg("%d", i, 1, IREG);
  }
  iregw = mkwildcard(ireg);
  for (i = 0; i < 32; i += 2) {
    freg2[i] = mkreg("%d", i, 3, FREG);
  }
  freg2w = mkwildcard(freg2);
  tmask[IREG] = INTTMP;
  vmask[IREG] = INTVAR;
  tmask[FREG] = FLTTMP;
  vmask[FREG] = FLTVAR;
}


static void progend(void) {
  /* nothing to do here */
}


static void segment(int s) {
  print("; *** segment(%d)\n", s);
}


static void space(int n) {
  print("; *** space(%d)\n", n);
}


static Symbol rmap(int opk) {
  print("; *** rmap(%d)\n", opk);
  return NULL;
}


static void blkfetch(int size, int off, int reg, int tmp) {
  print("; *** blkfetch(%d, %d, %d, %d)\n", size, off, reg, tmp);
}


static void blkstore(int size, int off, int reg, int tmp) {
  print("; *** blkstore(%d, %d, %d, %d)\n", size, off, reg, tmp);
}


static void blkloop(int dreg, int doff,
                    int sreg, int soff,
                    int size, int tmps[]) {
  print("; *** blkloop(%d, %d, %d, %d, %d, ...)\n",
        dreg, doff, sreg, soff, size);
}


static void emit2(Node p) {
  print("; *** emit2(%p)\n", p);
}


static void doarg(Node p) {
  print("; *** doarg(%p)\n", p);
}


static void target(Node p) {
  print("; *** target(%p)\n", p);
}


static void clobber(Node p) {
  print("; *** clobber(%p)\n", p);
}


static void _label(Node p) {
  print("; *** _label(%p)\n", p);
}


Interface eco32nIR = {
	1, 1, 0,	/* char */
	2, 2, 0,	/* short */
	4, 4, 0,	/* int */
	4, 4, 0,	/* long */
	4, 4, 0,	/* long long */
	4, 4, 1,	/* float */
	8, 4, 1,	/* double */
	8, 4, 1,	/* long double */
	4, 4, 0,	/* T* */
	0, 1, 0,	/* struct */
	0,		/* little_endian */
	0,		/* mulops_calls */
	0,		/* wants_callb */
	1,		/* wants_argb */
	1,		/* left_to_right */
	0,		/* wants_dag */
	0,		/* unsigned_char */
	address,
	blockbeg,
	blockend,
	defaddress,
	defconst,
	defstring,
	defsymbol,
	emit,
	export,
	function,
	gen,
	global,
	import,
	local,
	progbeg,
	progend,
	segment,
	space,
	0,		/* stabblock */
	0,		/* stabend */
	0,		/* stabfend */
	0,		/* stabinit */
	0,		/* stabline */
	0,		/* stabsym */
	0,		/* stabtype */
	{
	  1,		/* max_unaligned_load */
	  rmap,
	  blkfetch,
	  blkstore,
	  blkloop,
	  _label,
	  _rule,
	  _nts,
	  _kids,
	  _string,
	  _templates,
	  _isinstruction,
	  _ntname,
	  emit2,
	  doarg,
	  target,
	  clobber,
	}
};
