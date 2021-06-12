/*
 * instr.c -- instruction encoding
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "instr.h"


/*
 * This is the ECO32 machine instruction set.
 * The table below needs no particular order
 * and may have gaps in the instruction encoding.
 */
Instr instrTbl[] = {
  { "add",   FORMAT_RRY,  OP_ADD,  0,        NULL },
  { "sub",   FORMAT_RRY,  OP_SUB,  0,        NULL },
  { "mul",   FORMAT_RRY,  OP_MUL,  0,        NULL },
  { "mulu",  FORMAT_RRX,  OP_MULU, 0,        NULL },
  { "div",   FORMAT_RRY,  OP_DIV,  0,        NULL },
  { "divu",  FORMAT_RRX,  OP_DIVU, 0,        NULL },
  { "rem",   FORMAT_RRY,  OP_REM,  0,        NULL },
  { "remu",  FORMAT_RRX,  OP_REMU, 0,        NULL },
  { "and",   FORMAT_RRX,  OP_AND,  0,        NULL },
  { "or",    FORMAT_RRX,  OP_OR,   0,        NULL },
  { "xor",   FORMAT_RRX,  OP_XOR,  0,        NULL },
  { "xnor",  FORMAT_RRX,  OP_XNOR, 0,        NULL },
  { "sll",   FORMAT_RRX,  OP_SLL,  0,        NULL },
  { "slr",   FORMAT_RRX,  OP_SLR,  0,        NULL },
  { "sar",   FORMAT_RRX,  OP_SAR,  0,        NULL },
  { "dci",   FORMAT_XN,   OP_CCTL, XOP_DCI,  NULL },
  { "dcf",   FORMAT_XN,   OP_CCTL, XOP_DCF,  NULL },
  { "ici",   FORMAT_XN,   OP_CCTL, XOP_ICI,  NULL },
  { "cci",   FORMAT_XN,   OP_CCTL, XOP_CCI,  NULL },
  { "ccs",   FORMAT_XN,   OP_CCTL, XOP_CCS,  NULL },
  { "ldhi",  FORMAT_RHH,  OP_LDHI, 0,        NULL },
  { "beq",   FORMAT_RRB,  OP_BEQ,  0,        NULL },
  { "bne",   FORMAT_RRB,  OP_BNE,  0,        NULL },
  { "ble",   FORMAT_RRB,  OP_BLE,  0,        NULL },
  { "bleu",  FORMAT_RRB,  OP_BLEU, 0,        NULL },
  { "blt",   FORMAT_RRB,  OP_BLT,  0,        NULL },
  { "bltu",  FORMAT_RRB,  OP_BLTU, 0,        NULL },
  { "beqf",  FORMAT_RRB,  OP_BEQF, 0,        NULL },
  { "bnef",  FORMAT_RRB,  OP_BNEF, 0,        NULL },
  { "blef",  FORMAT_RRB,  OP_BLEF, 0,        NULL },
  { "bltf",  FORMAT_RRB,  OP_BLTF, 0,        NULL },
  { "j",     FORMAT_J,    OP_J,    0,        NULL },
  { "jr",    FORMAT_JR,   OP_JR,   0,        NULL },
  { "jal",   FORMAT_J,    OP_JAL,  0,        NULL },
  { "jalr",  FORMAT_JR,   OP_JALR, 0,        NULL },
  { "trap",  FORMAT_N,    OP_TRAP, 0,        NULL },
  { "rfx",   FORMAT_N,    OP_RFX,  0,        NULL },
  { "ldw",   FORMAT_RRS,  OP_LDW,  0,        NULL },
  { "ldh",   FORMAT_RRS,  OP_LDH,  0,        NULL },
  { "ldhu",  FORMAT_RRS,  OP_LDHU, 0,        NULL },
  { "ldb",   FORMAT_RRS,  OP_LDB,  0,        NULL },
  { "ldbu",  FORMAT_RRS,  OP_LDBU, 0,        NULL },
  { "stw",   FORMAT_RRS,  OP_STW,  0,        NULL },
  { "sth",   FORMAT_RRS,  OP_STH,  0,        NULL },
  { "stb",   FORMAT_RRS,  OP_STB,  0,        NULL },
  { "mvfs",  FORMAT_RH,   OP_MVFS, 0,        NULL },
  { "mvts",  FORMAT_RH,   OP_MVTS, 0,        NULL },
  { "tbs",   FORMAT_XN,   OP_TCTL, XOP_TBS,  NULL },
  { "tbwr",  FORMAT_XN,   OP_TCTL, XOP_TBWR, NULL },
  { "tbri",  FORMAT_XN,   OP_TCTL, XOP_TBRI, NULL },
  { "tbwi",  FORMAT_XN,   OP_TCTL, XOP_TBWI, NULL },
  { "addf",  FORMAT_XRRR, OP_FPAR, XOP_ADDF, NULL },
  { "subf",  FORMAT_XRRR, OP_FPAR, XOP_SUBF, NULL },
  { "mulf",  FORMAT_XRRR, OP_FPAR, XOP_MULF, NULL },
  { "divf",  FORMAT_XRRR, OP_FPAR, XOP_DIVF, NULL },
  { "cf2i",  FORMAT_XRR,  OP_FPAR, XOP_CF2I, NULL },
  { "ci2f",  FORMAT_XRR,  OP_FPAR, XOP_CI2F, NULL },
  { "ldlw",  FORMAT_RRS,  OP_LDLW, 0,        NULL },
  { "stcw",  FORMAT_RRS,  OP_STCW, 0,        NULL },
};


Instr *instrCodeTbl[64];


static int instrCompare(const void *instr1, const void *instr2) {
  return strcmp(((Instr *) instr1)->name, ((Instr *) instr2)->name);
}


void initInstrTable(void) {
  int i;

  /* first sort instruction table alphabetically */
  qsort(instrTbl, sizeof(instrTbl)/sizeof(instrTbl[0]),
        sizeof(instrTbl[0]), instrCompare);
  /* then initialize instruction code table */
  for (i = 0; i < 64; i++) {
    instrCodeTbl[i] = NULL;
  }
  for (i = 0; i < sizeof(instrTbl)/sizeof(instrTbl[0]); i++) {
    /* build link to other extended instrs with same main opcode */
    instrTbl[i].alt = instrCodeTbl[instrTbl[i].opcode];
    /* let instr code table entry point to instr table entry */
    instrCodeTbl[instrTbl[i].opcode] = &instrTbl[i];
    if (instrTbl[i].format == FORMAT_RRX ||
        instrTbl[i].format == FORMAT_RRY) {
      /* enter the immediate variant of this instruction also */
      instrCodeTbl[instrTbl[i].opcode + 1] = &instrTbl[i];
    }
  }
}


Instr *lookupInstr(char *name) {
  int lo, hi, tst;
  int res;

  lo = 0;
  hi = sizeof(instrTbl) / sizeof(instrTbl[0]) - 1;
  while (lo <= hi) {
    tst = (lo + hi) / 2;
    res = strcmp(instrTbl[tst].name, name);
    if (res == 0) {
      return &instrTbl[tst];
    }
    if (res < 0) {
      lo = tst + 1;
    } else {
      hi = tst - 1;
    }
  }
  return NULL;
}
