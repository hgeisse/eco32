/*
 * disasm.c -- disassembler
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "instr.h"
#include "disasm.h"


static char instrBuffer[100];


static void disasmN(char *opName, Word immed) {
  if (immed == 0) {
    sprintf(instrBuffer, "%-7s", opName);
  } else {
    sprintf(instrBuffer, "%-7s %08X", opName, immed);
  }
}


static void disasmRH(char *opName, int r1, Half immed) {
  sprintf(instrBuffer, "%-7s $%d,%04X", opName, r1, immed);
}


static void disasmRHH(char *opName, int r1, Half immed) {
  sprintf(instrBuffer, "%-7s $%d,%08X", opName, r1, (Word) immed << 16);
}


static void disasmRRH(char *opName, int r1, int r2, Half immed) {
  sprintf(instrBuffer, "%-7s $%d,$%d,%04X", opName, r1, r2, immed);
}


static void disasmRRS(char *opName, int r1, int r2, Half immed) {
  sprintf(instrBuffer, "%-7s $%d,$%d,%s%04X", opName, r1, r2,
          SIGN(16) & immed ? "-" : "+",
          SIGN(16) & immed ? -SEXT16(immed) : SEXT16(immed));
}


static void disasmRRR(char *opName, int r1, int r2, int r3) {
  sprintf(instrBuffer, "%-7s $%d,$%d,$%d", opName, r1, r2, r3);
}


static void disasmRRB(char *opName, int r1, int r2, Half offset, Word locus) {
  sprintf(instrBuffer, "%-7s $%d,$%d,%08X", opName,
          r1, r2, (locus + 4) + (SEXT16(offset) << 2));
}


static void disasmJ(char *opName, Word offset, Word locus) {
  sprintf(instrBuffer, "%-7s %08X", opName,
          (locus + 4) + (SEXT26(offset) << 2));
}


static void disasmJR(char *opName, int r1) {
  sprintf(instrBuffer, "%-7s $%d", opName, r1);
}


static void disasmXN(char *opName) {
  sprintf(instrBuffer, "%-7s", opName);
}


static void disasmXRR(char *opName, int r1, int r2) {
  sprintf(instrBuffer, "%-7s $%d,$%d", opName, r1, r2);
}


static void disasmXRRR(char *opName, int r1, int r2, int r3) {
  sprintf(instrBuffer, "%-7s $%d,$%d,$%d", opName, r1, r2, r3);
}


static void disasmExtended(Instr *ip, Word instr) {
  Byte xopcode;

  xopcode = instr & 0x000000FF;
  while (ip != NULL && ip->xopcode != xopcode) {
    ip = ip->alt;
  }
  if (ip == NULL) {
    disasmN("???", 0);
  } else {
    switch (ip->format) {
      case FORMAT_XN:
        disasmXN(ip->name);
        break;
      case FORMAT_XRR:
        disasmXRR(ip->name, (instr >> 16) & 0x1F,
                  (instr >> 21) & 0x1F);
        break;
      case FORMAT_XRRR:
        disasmXRRR(ip->name, (instr >> 11) & 0x1F,
                   (instr >> 21) & 0x1F, (instr >> 16) & 0x1F);
        break;
      default:
        error("illegal extended entry in instruction table");
    }
  }
}


char *disasm(Word instr, Word locus) {
  Byte opcode;
  Instr *ip;

  opcode = (instr >> 26) & 0x3F;
  ip = instrCodeTbl[opcode];
  if (ip == NULL) {
    disasmN("???", 0);
  } else {
    switch (ip->format) {
      case FORMAT_N:
        disasmN(ip->name, instr & 0x03FFFFFF);
        break;
      case FORMAT_RH:
        disasmRH(ip->name, (instr >> 16) & 0x1F, instr & 0x0000FFFF);
        break;
      case FORMAT_RHH:
        disasmRHH(ip->name, (instr >> 16) & 0x1F, instr & 0x0000FFFF);
        break;
      case FORMAT_RRH:
        disasmRRH(ip->name, (instr >> 16) & 0x1F,
                  (instr >> 21) & 0x1F, instr & 0x0000FFFF);
        break;
      case FORMAT_RRS:
        disasmRRS(ip->name, (instr >> 16) & 0x1F,
                  (instr >> 21) & 0x1F, instr & 0x0000FFFF);
        break;
      case FORMAT_RRR:
        disasmRRR(ip->name, (instr >> 11) & 0x1F,
                  (instr >> 21) & 0x1F, (instr >> 16) & 0x1F);
        break;
      case FORMAT_RRX:
        if ((opcode & 1) == 0) {
          /* the FORMAT_RRR variant */
          disasmRRR(ip->name, (instr >> 11) & 0x1F,
                    (instr >> 21) & 0x1F, (instr >> 16) & 0x1F);
        } else {
          /* the FORMAT_RRH variant */
          disasmRRH(ip->name, (instr >> 16) & 0x1F,
                    (instr >> 21) & 0x1F, instr & 0x0000FFFF);
        }
        break;
      case FORMAT_RRY:
        if ((opcode & 1) == 0) {
          /* the FORMAT_RRR variant */
          disasmRRR(ip->name, (instr >> 11) & 0x1F,
                    (instr >> 21) & 0x1F, (instr >> 16) & 0x1F);
        } else {
          /* the FORMAT_RRS variant */
          disasmRRS(ip->name, (instr >> 16) & 0x1F,
                    (instr >> 21) & 0x1F, instr & 0x0000FFFF);
        }
        break;
      case FORMAT_RRB:
        disasmRRB(ip->name, (instr >> 21) & 0x1F,
                  (instr >> 16) & 0x1F, instr & 0x0000FFFF, locus);
        break;
      case FORMAT_J:
        disasmJ(ip->name, instr & 0x03FFFFFF, locus);
        break;
      case FORMAT_JR:
        disasmJR(ip->name, (instr >> 21) & 0x1F);
        break;
      case FORMAT_XN:
      case FORMAT_XRR:
      case FORMAT_XRRR:
        disasmExtended(ip, instr);
        break;
      default:
        error("illegal entry in instruction table");
    }
  }
  return instrBuffer;
}
