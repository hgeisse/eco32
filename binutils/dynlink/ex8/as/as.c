/*
 * as.c -- ECO32 assembler
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>

#include "eof.h"


/**************************************************************/


#define NUM_REGS	32
#define AUX_REG		1
#define GOT_REG		25

#define LINE_SIZE	200

#define TOK_EOL		0
#define TOK_LABEL	1
#define TOK_IDENT	2
#define TOK_STRING	3
#define TOK_NUMBER	4
#define TOK_REGISTER	5
#define TOK_PLUS	6
#define TOK_MINUS	7
#define TOK_STAR	8
#define TOK_SLASH	9
#define TOK_PERCENT	10
#define TOK_LSHIFT	11
#define TOK_RSHIFT	12
#define TOK_LPAREN	13
#define TOK_RPAREN	14
#define TOK_COMMA	15
#define TOK_TILDE	16
#define TOK_AMPER	17
#define TOK_BAR		18
#define TOK_CARET	19

#define STATUS_UNKNOWN	0	/* symbol is not yet defined */
#define STATUS_DEFINED	1	/* symbol is defined */
#define STATUS_GLOBREF	2	/* local entry refers to a global one */

#define GLOBAL_TABLE	0	/* global symbol table identifier */
#define LOCAL_TABLE	1	/* local symbol table identifier */

#define MSB	((unsigned int) 1 << (sizeof(unsigned int) * 8 - 1))


/**************************************************************/


#define OP_ADD		0x00
#define OP_ADDI		0x01
#define OP_SUB		0x02
#define OP_SUBI		0x03

#define OP_MUL		0x04
#define OP_MULI		0x05
#define OP_MULU		0x06
#define OP_MULUI	0x07
#define OP_DIV		0x08
#define OP_DIVI		0x09
#define OP_DIVU		0x0A
#define OP_DIVUI	0x0B
#define OP_REM		0x0C
#define OP_REMI		0x0D
#define OP_REMU		0x0E
#define OP_REMUI	0x0F

#define OP_AND		0x10
#define OP_ANDI		0x11
#define OP_OR		0x12
#define OP_ORI		0x13
#define OP_XOR		0x14
#define OP_XORI		0x15
#define OP_XNOR		0x16
#define OP_XNORI	0x17

#define OP_SLL		0x18
#define OP_SLLI		0x19
#define OP_SLR		0x1A
#define OP_SLRI		0x1B
#define OP_SAR		0x1C
#define OP_SARI		0x1D

#define OP_CCTL		0x1E

#define OP_LDHI		0x1F

#define OP_BEQ		0x20
#define OP_BNE		0x21
#define OP_BLE		0x22
#define OP_BLEU		0x23
#define OP_BLT		0x24
#define OP_BLTU		0x25
#define OP_BGE		0x26
#define OP_BGEU		0x27
#define OP_BGT		0x28
#define OP_BGTU		0x29

#define OP_J		0x2A
#define OP_JR		0x2B
#define OP_JAL		0x2C
#define OP_JALR		0x2D

#define OP_TRAP		0x2E
#define OP_RFX		0x2F

#define OP_LDW		0x30
#define OP_LDH		0x31
#define OP_LDHU		0x32
#define OP_LDB		0x33
#define OP_LDBU		0x34

#define OP_STW		0x35
#define OP_STH		0x36
#define OP_STB		0x37

#define OP_MVFS		0x38
#define OP_MVTS		0x39

#define OP_TBS		0x3A
#define OP_TBWR		0x3B
#define OP_TBRI		0x3C
#define OP_TBWI		0x3D

#define OP_LDLW		0x3E
#define OP_STCW		0x3F


/**************************************************************/


#define SEGMENT_ABS	0	/* absolute values */
#define SEGMENT_CODE	1	/* code segment */
#define SEGMENT_DATA	2	/* initialized data segment */
#define SEGMENT_BSS	3	/* uninitialized data segment */


int debugCmdline = 0;
int debugToken = 0;
int debugCode = 0;
int debugFixup = 0;
int debugModule = 0;

int genPIC = 0;
int gotReg = GOT_REG;

char codeName[L_tmpnam];
char dataName[L_tmpnam];
char *outName = NULL;
char *inName = NULL;

FILE *codeFile = NULL;
FILE *dataFile = NULL;
FILE *outFile = NULL;
FILE *inFile = NULL;

char line[LINE_SIZE];
char *lineptr;
int lineno;

int token;
int tokenvalNumber;
char tokenvalString[LINE_SIZE];

int allowSyn = 1;
int currSeg = SEGMENT_CODE;
unsigned int segPtr[4] = { 0, 0, 0, 0 };
char *segName[4] = { "ABS", "CODE", "DATA", "BSS" };
char *methodName[10] = {
  "H16", "L16", "R16", "R26", "W32",
  "GOTADRH", "GOTADRL", "GOTOFFH", "GOTOFFL", "GOTPNTR"
};


typedef struct fixup {
  int segment;			/* in which segment */
  unsigned int offset;		/* at which offset */
  int method;			/* what kind of coding method is to be used */
  int value;			/* known part of value */
  int base;			/* segment which this ref is relative to */
				/* valid only when used for relocation */
  struct fixup *next;		/* next fixup */
} Fixup;


typedef struct symbol {
  char *name;			/* name of symbol */
  int whichTable;		/* GLOBAL_TABLE or LOCAL_TABLE */
  int status;			/* status of symbol */
  int segment;			/* the symbol's segment */
  int value;			/* the symbol's value */
  Fixup *fixups;		/* list of locations to fix */
  struct symbol *globref;	/* set if this local refers to a global */
  struct symbol *left;		/* left son in binary search tree */
  struct symbol *right;		/* right son in binary search tree */
  int skip;			/* this symbol is not defined here nor is */
				/* it used here: don't write to object file */
} Symbol;


/**************************************************************/


void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  fprintf(stderr, "Error: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
  if (codeFile != NULL) {
    fclose(codeFile);
    codeFile = NULL;
  }
  if (dataFile != NULL) {
    fclose(dataFile);
    dataFile = NULL;
  }
  if (outFile != NULL) {
    fclose(outFile);
    outFile = NULL;
  }
  if (inFile != NULL) {
    fclose(inFile);
    inFile = NULL;
  }
  if (codeName != NULL) {
    unlink(codeName);
  }
  if (dataName != NULL) {
    unlink(dataName);
  }
  if (outName != NULL) {
    unlink(outName);
  }
  exit(1);
}


void *allocateMemory(unsigned int size) {
  void *p;

  p = malloc(size);
  if (p == NULL) {
    error("out of memory");
  }
  return p;
}


void freeMemory(void *p) {
  free(p);
}


/**************************************************************/


int getNextToken(void) {
  char *p;
  int base;
  int digit;

  while (*lineptr == ' ' || *lineptr == '\t') {
    lineptr++;
  }
  if (*lineptr == '\n' || *lineptr == '\0' || *lineptr == ';') {
    return TOK_EOL;
  }
  if (isalpha((int) *lineptr) || *lineptr == '_' || *lineptr == '.') {
    p = tokenvalString;
    while (isalnum((int) *lineptr) || *lineptr == '_' || *lineptr == '.') {
      *p++ = *lineptr++;
    }
    *p = '\0';
    if (*lineptr == ':') {
      lineptr++;
      return TOK_LABEL;
    } else {
      return TOK_IDENT;
    }
  }
  if (isdigit((int) *lineptr)) {
    base = 10;
    tokenvalNumber = 0;
    if (*lineptr == '0') {
      lineptr++;
      if (*lineptr == 'x' || *lineptr == 'X') {
        base = 16;
        lineptr++;
      } else
      if (isdigit((int) *lineptr)) {
        base = 8;
      } else {
        return TOK_NUMBER;
      }
    }
    while (isxdigit((int) *lineptr)) {
      digit = *lineptr++ - '0';
      if (digit >= 'A' - '0') {
        if (digit >= 'a' - '0') {
          digit += '0' - 'a' + 10;
        } else {
          digit += '0' - 'A' + 10;
        }
      }
      if (digit >= base) {
        error("illegal digit value %d in line %d", digit, lineno);
      }
      tokenvalNumber *= base;
      tokenvalNumber += digit;
    }
    return TOK_NUMBER;
  }
  if (*lineptr == '\'') {
    lineptr++;
    if (!isprint((int) *lineptr)) {
      error("cannot quote character 0x%02X in line %d", *lineptr, lineno);
    }
    tokenvalNumber = *lineptr;
    lineptr++;
    if (*lineptr != '\'') {
      error("unbalanced quote in line %d", lineno);
    }
    lineptr++;
    return TOK_NUMBER;
  }
  if (*lineptr == '\"') {
    lineptr++;
    p = tokenvalString;
    while (1) {
      if (*lineptr == '\n' || *lineptr == '\0') {
        error("unterminated string constant in line %d", lineno);
      }
      if (!isprint((int) *lineptr)) {
        error("string contains illegal character 0x%02X in line %d",
              *lineptr, lineno);
      }
      if (*lineptr == '\"') {
        break;
      }
      *p++ = *lineptr++;
    }
    lineptr++;
    *p = '\0';
    return TOK_STRING;
  }
  if (*lineptr == '$') {
    lineptr++;
    if (!isdigit((int) *lineptr)) {
      error("register number expected after '$' in line %d", lineno);
    }
    tokenvalNumber = 0;
    while (isdigit((int) *lineptr)) {
      digit = *lineptr++ - '0';
      tokenvalNumber *= 10;
      tokenvalNumber += digit;
    }
    if (tokenvalNumber < 0 || tokenvalNumber >= NUM_REGS) {
      error("illegal register number %d in line %d", tokenvalNumber, lineno);
    }
    return TOK_REGISTER;
  }
  if (*lineptr == '+') {
    lineptr++;
    return TOK_PLUS;
  }
  if (*lineptr == '-') {
    lineptr++;
    return TOK_MINUS;
  }
  if (*lineptr == '*') {
    lineptr++;
    return TOK_STAR;
  }
  if (*lineptr == '/') {
    lineptr++;
    return TOK_SLASH;
  }
  if (*lineptr == '%') {
    lineptr++;
    return TOK_PERCENT;
  }
  if (*lineptr == '<' && *(lineptr + 1) == '<') {
    lineptr += 2;
    return TOK_LSHIFT;
  }
  if (*lineptr == '>' && *(lineptr + 1) == '>') {
    lineptr += 2;
    return TOK_RSHIFT;
  }
  if (*lineptr == '(') {
    lineptr++;
    return TOK_LPAREN;
  }
  if (*lineptr == ')') {
    lineptr++;
    return TOK_RPAREN;
  }
  if (*lineptr == ',') {
    lineptr++;
    return TOK_COMMA;
  }
  if (*lineptr == '~') {
    lineptr++;
    return TOK_TILDE;
  }
  if (*lineptr == '&') {
    lineptr++;
    return TOK_AMPER;
  }
  if (*lineptr == '|') {
    lineptr++;
    return TOK_BAR;
  }
  if (*lineptr == '^') {
    lineptr++;
    return TOK_CARET;
  }
  error("illegal character 0x%02X in line %d", *lineptr, lineno);
  return 0;
}


void showToken(void) {
  printf("DEBUG: ");
  switch (token) {
    case TOK_EOL:
      printf("token = TOK_EOL\n");
      break;
    case TOK_LABEL:
      printf("token = TOK_LABEL, value = %s\n", tokenvalString);
      break;
    case TOK_IDENT:
      printf("token = TOK_IDENT, value = %s\n", tokenvalString);
      break;
    case TOK_STRING:
      printf("token = TOK_STRING, value = %s\n", tokenvalString);
      break;
    case TOK_NUMBER:
      printf("token = TOK_NUMBER, value = 0x%x\n", tokenvalNumber);
      break;
    case TOK_REGISTER:
      printf("token = TOK_REGISTER, value = %d\n", tokenvalNumber);
      break;
    case TOK_PLUS:
      printf("token = TOK_PLUS\n");
      break;
    case TOK_MINUS:
      printf("token = TOK_MINUS\n");
      break;
    case TOK_STAR:
      printf("token = TOK_STAR\n");
      break;
    case TOK_SLASH:
      printf("token = TOK_SLASH\n");
      break;
    case TOK_PERCENT:
      printf("token = TOK_PERCENT\n");
      break;
    case TOK_LSHIFT:
      printf("token = TOK_LSHIFT\n");
      break;
    case TOK_RSHIFT:
      printf("token = TOK_RSHIFT\n");
      break;
    case TOK_LPAREN:
      printf("token = TOK_LPAREN\n");
      break;
    case TOK_RPAREN:
      printf("token = TOK_RPAREN\n");
      break;
    case TOK_COMMA:
      printf("token = TOK_COMMA\n");
      break;
    case TOK_TILDE:
      printf("token = TOK_TILDE\n");
      break;
    case TOK_AMPER:
      printf("token = TOK_AMPER\n");
      break;
    case TOK_BAR:
      printf("token = TOK_BAR\n");
      break;
    case TOK_CARET:
      printf("token = TOK_CARET\n");
      break;
    default:
      error("illegal token %d in showToken()", token);
  }
}


void getToken(void) {
  token = getNextToken();
  if (debugToken) {
    showToken();
  }
}


static char *tok2str[] = {
  "end-of-line",
  "label",
  "identifier",
  "string",
  "number",
  "register",
  "+",
  "-",
  "*",
  "/",
  "%",
  "<<",
  ">>",
  "(",
  ")",
  ",",
  "~",
  "&",
  "|",
  "^"
};


void expect(int expected) {
  if (token != expected) {
    error("'%s' expected, got '%s' in line %d",
          tok2str[expected], tok2str[token], lineno);
  }
}


/**************************************************************/


Fixup *fixupList = NULL;


Fixup *newFixup(int segment, unsigned int offset, int method, int value) {
  Fixup *f;

  f = allocateMemory(sizeof(Fixup));
  f->segment = segment;
  f->offset = offset;
  f->method = method;
  f->value = value;
  f->base = 0;
  f->next = NULL;
  return f;
}


void addFixupToSym(Symbol *s,
                   int segment, unsigned int offset,
                   int method, int value) {
  Fixup *f;

  if (debugFixup) {
    printf("DEBUG: fixup (s:%s, o:%08X, m:%s, v:%08X) added to '%s'\n",
           segName[segment], offset, methodName[method], value, s->name);
  }
  f = newFixup(segment, offset, method, value);
  f->next = s->fixups;
  s->fixups = f;
}


void addFixupToLst(int segment, unsigned int offset,
                   int method, int value) {
  Fixup *f;

  if (debugFixup) {
    printf("DEBUG: fixup (s:%s, o:%08X, m:%s, v:%08X) added to fixup list\n",
           segName[segment], offset, methodName[method], value);
  }
  f = newFixup(segment, offset, method, value);
  f->next = fixupList;
  fixupList = f;
}


/**************************************************************/


Symbol *globalTable = NULL;
Symbol *localTable = NULL;


Symbol *deref(Symbol *s) {
  if (s->status == STATUS_GLOBREF) {
    return s->globref;
  } else {
    return s;
  }
}


Symbol *newSymbol(char *name, int whichTable) {
  Symbol *p;

  p = allocateMemory(sizeof(Symbol));
  p->name = allocateMemory(strlen(name) + 1);
  strcpy(p->name, name);
  p->whichTable = whichTable;
  p->status = STATUS_UNKNOWN;
  p->segment = 0;
  p->value = 0;
  p->fixups = NULL;
  p->globref = NULL;
  p->left = NULL;
  p->right = NULL;
  return p;
}


Symbol *lookupEnter(char *name, int whichTable) {
  Symbol *p, *q, *r;
  int cmp;

  if (whichTable == GLOBAL_TABLE) {
    p = globalTable;
  } else {
    p = localTable;
  }
  if (p == NULL) {
    r = newSymbol(name, whichTable);
    if (whichTable == GLOBAL_TABLE) {
      globalTable = r;
    } else {
      localTable = r;
    }
    return r;
  }
  while (1) {
    q = p;
    cmp = strcmp(name, q->name);
    if (cmp == 0) {
      return q;
    }
    if (cmp < 0) {
      p = q->left;
    } else {
      p = q->right;
    }
    if (p == NULL) {
      r = newSymbol(name, whichTable);
      if (cmp < 0) {
        q->left = r;
      } else {
        q->right = r;
      }
      return r;
    }
  }
  /* never reached */
  return NULL;
}


static void linkSymbol(Symbol *s) {
  Fixup *f;

  if (s->status == STATUS_UNKNOWN) {
    error("undefined symbol '%s'", s->name);
  }
  if (s->status == STATUS_GLOBREF) {
    if (s->fixups != NULL) {
      error("local fixups detected with global symbol '%s'", s->name);
    }
  } else {
    if (debugFixup) {
      printf("DEBUG: link '%s' (s:%s, v:%08X)\n",
             s->name, segName[s->segment], s->value);
    }
    while (s->fixups != NULL) {
      /* get next fixup record */
      f = s->fixups;
      s->fixups = f->next;
      /* add the symbol's value to the value in the record */
      /* and remember the symbol's segment */
      if (debugFixup) {
        printf("       (s:%s, o:%08X, m:%s, v:%08X --> %08X, b:%s)\n",
               segName[f->segment], f->offset, methodName[f->method],
               f->value, f->value + s->value, segName[s->segment]);
      }
      f->value += s->value;
      f->base = s->segment;
      /* transfer the record to the fixup list */
      f->next = fixupList;
      fixupList = f;
    }
  }
}


static void linkTree(Symbol *s) {
  if (s == NULL) {
    return;
  }
  linkTree(s->left);
  linkSymbol(s);
  linkTree(s->right);
  freeMemory(s->name);
  freeMemory(s);
}


void linkLocals(void) {
  linkTree(localTable);
  localTable = NULL;
  fseek(codeFile, 0, SEEK_END);
  fseek(dataFile, 0, SEEK_END);
}


/**************************************************************/


static int numSymbols;


void walkTree(Symbol *s, void (*fp)(Symbol *sp)) {
  if (s == NULL) {
    return;
  }
  walkTree(s->left, fp);
  (*fp)(s);
  walkTree(s->right, fp);
}


void transferFixupsForSymbol(Symbol *s) {
  Fixup *f;

  if (s->status != STATUS_UNKNOWN && s->status != STATUS_DEFINED) {
    /* this should never happen */
    error("global symbol is neither unknown nor defined");
  }
  if (s->status == STATUS_UNKNOWN && s->fixups == NULL) {
    /* this symbol is neither defined here nor referenced here: skip */
    s->skip = 1;
    return;
  }
  s->skip = 0;
  while (s->fixups != NULL) {
    /* get next fixup record */
    f = s->fixups;
    s->fixups = f->next;
    /* use the 'base' component to store the current symbol number */
    f->base = MSB | numSymbols;
    /* transfer the record to the fixup list */
    f->next = fixupList;
    fixupList = f;
  }
  numSymbols++;
}


void transferFixups(void) {
  numSymbols = 0;
  walkTree(globalTable, transferFixupsForSymbol);
}


/**************************************************************/


void emitByte(unsigned int byte) {
  byte &= 0x000000FF;
  if (debugCode) {
    printf("DEBUG: byte @ segment = %s, offset = %08X",
           segName[currSeg], segPtr[currSeg]);
    printf(", value = %02X\n", byte);
  }
  switch (currSeg) {
    case SEGMENT_ABS:
      error("illegal segment in emitByte()");
      break;
    case SEGMENT_CODE:
      fputc(byte, codeFile);
      break;
    case SEGMENT_DATA:
      fputc(byte, dataFile);
      break;
    case SEGMENT_BSS:
      break;
  }
  segPtr[currSeg] += 1;
}


void emitHalf(unsigned int half) {
  half &= 0x0000FFFF;
  if (debugCode) {
    printf("DEBUG: half @ segment = %s, offset = %08X",
           segName[currSeg], segPtr[currSeg]);
    printf(", value = %02X%02X\n",
           (half >> 8) & 0xFF, half & 0xFF);
  }
  switch (currSeg) {
    case SEGMENT_ABS:
      error("illegal segment in emitHalf()");
      break;
    case SEGMENT_CODE:
      fputc((half >> 8) & 0xFF, codeFile);
      fputc(half & 0xFF, codeFile);
      break;
    case SEGMENT_DATA:
      fputc((half >> 8) & 0xFF, dataFile);
      fputc(half & 0xFF, dataFile);
      break;
    case SEGMENT_BSS:
      break;
  }
  segPtr[currSeg] += 2;
}


void emitWord(unsigned int word) {
  if (debugCode) {
    printf("DEBUG: word @ segment = %s, offset = %08X",
           segName[currSeg], segPtr[currSeg]);
    printf(", value = %02X%02X%02X%02X\n",
           (word >> 24) & 0xFF, (word >> 16) & 0xFF,
           (word >> 8) & 0xFF, word & 0xFF);
  }
  switch (currSeg) {
    case SEGMENT_ABS:
      error("illegal segment in emitWord()");
      break;
    case SEGMENT_CODE:
      fputc((word >> 24) & 0xFF, codeFile);
      fputc((word >> 16) & 0xFF, codeFile);
      fputc((word >> 8) & 0xFF, codeFile);
      fputc(word & 0xFF, codeFile);
      break;
    case SEGMENT_DATA:
      fputc((word >> 24) & 0xFF, dataFile);
      fputc((word >> 16) & 0xFF, dataFile);
      fputc((word >> 8) & 0xFF, dataFile);
      fputc(word & 0xFF, dataFile);
      break;
    case SEGMENT_BSS:
      break;
  }
  segPtr[currSeg] += 4;
}


/**************************************************************/


typedef struct {
  int con;
  Symbol *sym;
} Value;


Value parseExpression(void);


Value parsePrimaryExpression(void) {
  Value v;
  Symbol *s;

  if (token == TOK_NUMBER) {
    v.con = tokenvalNumber;
    v.sym = NULL;
    getToken();
  } else
  if (token == TOK_IDENT) {
    s = deref(lookupEnter(tokenvalString, LOCAL_TABLE));
    if (s->status == STATUS_DEFINED && s->segment == SEGMENT_ABS) {
      v.con = s->value;
      v.sym = NULL;
    } else {
      v.con = 0;
      v.sym = s;
    }
    getToken();
  } else
  if (token == TOK_LPAREN) {
    getToken();
    v = parseExpression();
    expect(TOK_RPAREN);
    getToken();
  } else {
    error("illegal primary expression, line %d", lineno);
  }
  return v;
}


Value parseUnaryExpression(void) {
  Value v;

  if (token == TOK_PLUS) {
    getToken();
    v = parseUnaryExpression();
  } else
  if (token == TOK_MINUS) {
    getToken();
    v = parseUnaryExpression();
    if (v.sym != NULL) {
      error("cannot negate symbol '%s' in line %d", v.sym->name, lineno);
    }
    v.con = -v.con;
  } else
  if (token == TOK_TILDE) {
    getToken();
    v = parseUnaryExpression();
    if (v.sym != NULL) {
      error("cannot complement symbol '%s' in line %d", v.sym->name, lineno);
    }
    v.con = ~v.con;
  } else {
    v = parsePrimaryExpression();
  }
  return v;
}


Value parseMultiplicativeExpression(void) {
  Value v1, v2;

  v1 = parseUnaryExpression();
  while (token == TOK_STAR || token == TOK_SLASH || token == TOK_PERCENT) {
    if (token == TOK_STAR) {
      getToken();
      v2 = parseUnaryExpression();
      if (v1.sym != NULL || v2.sym != NULL) {
        error("multiplication of symbols not supported, line %d", lineno);
      }
      v1.con *= v2.con;
    } else
    if (token == TOK_SLASH) {
      getToken();
      v2 = parseUnaryExpression();
      if (v1.sym != NULL || v2.sym != NULL) {
        error("division of symbols not supported, line %d", lineno);
      }
      if (v2.con == 0) {
        error("division by zero, line %d", lineno);
      }
      v1.con /= v2.con;
    } else
    if (token == TOK_PERCENT) {
      getToken();
      v2 = parseUnaryExpression();
      if (v1.sym != NULL || v2.sym != NULL) {
        error("division of symbols not supported, line %d", lineno);
      }
      if (v2.con == 0) {
        error("division by zero, line %d", lineno);
      }
      v1.con %= v2.con;
    }
  }
  return v1;
}


Value parseAdditiveExpression(void) {
  Value v1, v2;

  v1 = parseMultiplicativeExpression();
  while (token == TOK_PLUS || token == TOK_MINUS) {
    if (token == TOK_PLUS) {
      getToken();
      v2 = parseMultiplicativeExpression();
      if (v1.sym != NULL && v2.sym != NULL) {
        error("addition of symbols not supported, line %d", lineno);
      }
      if (v2.sym != NULL) {
        v1.sym = v2.sym;
      }
      v1.con += v2.con;
    } else
    if (token == TOK_MINUS) {
      getToken();
      v2 = parseMultiplicativeExpression();
      if (v2.sym != NULL) {
        error("subtraction of symbols not supported, line %d", lineno);
      }
      v1.con -= v2.con;
    }
  }
  return v1;
}


Value parseShiftExpression(void) {
  Value v1, v2;

  v1 = parseAdditiveExpression();
  while (token == TOK_LSHIFT || token == TOK_RSHIFT) {
    if (token == TOK_LSHIFT) {
      getToken();
      v2 = parseAdditiveExpression();
      if (v1.sym != NULL || v2.sym != NULL) {
        error("shifting of symbols not supported, line %d", lineno);
      }
      v1.con <<= v2.con;
    } else
    if (token == TOK_RSHIFT) {
      getToken();
      v2 = parseAdditiveExpression();
      if (v1.sym != NULL || v2.sym != NULL) {
        error("shifting of symbols not supported, line %d", lineno);
      }
      v1.con >>= v2.con;
    }
  }
  return v1;
}


Value parseAndExpression(void) {
  Value v1, v2;

  v1 = parseShiftExpression();
  while (token == TOK_AMPER) {
    getToken();
    v2 = parseShiftExpression();
    if (v2.sym != NULL) {
      error("bitwise 'and' of symbols not supported, line %d", lineno);
    }
    v1.con &= v2.con;
  }
  return v1;
}


Value parseExclusiveOrExpression(void) {
  Value v1, v2;

  v1 = parseAndExpression();
  while (token == TOK_CARET) {
    getToken();
    v2 = parseAndExpression();
    if (v2.sym != NULL) {
      error("bitwise 'xor' of symbols not supported, line %d", lineno);
    }
    v1.con ^= v2.con;
  }
  return v1;
}


Value parseInclusiveOrExpression(void) {
  Value v1, v2;

  v1 = parseExclusiveOrExpression();
  while (token == TOK_BAR) {
    getToken();
    v2 = parseExclusiveOrExpression();
    if (v2.sym != NULL) {
      error("bitwise 'or' of symbols not supported, line %d", lineno);
    }
    v1.con |= v2.con;
  }
  return v1;
}


Value parseExpression(void) {
  Value v;

  v = parseInclusiveOrExpression();
  return v;
}


/**************************************************************/


void dotSyn(unsigned int code) {
  allowSyn = 1;
}


void dotNosyn(unsigned int code) {
  allowSyn = 0;
}


void dotCode(unsigned int code) {
  currSeg = SEGMENT_CODE;
}


void dotData(unsigned int code) {
  currSeg = SEGMENT_DATA;
}


void dotBss(unsigned int code) {
  currSeg = SEGMENT_BSS;
}


void dotExport(unsigned int code) {
  Symbol *global;
  Symbol *local;
  Fixup *f;

  while (1) {
    expect(TOK_IDENT);
    global = lookupEnter(tokenvalString, GLOBAL_TABLE);
    if (global->status != STATUS_UNKNOWN) {
      error("exported symbol '%s' multiply defined in line %d",
            global->name, lineno);
    }
    local = lookupEnter(tokenvalString, LOCAL_TABLE);
    if (local->status == STATUS_GLOBREF) {
      error("exported symbol '%s' multiply exported in line %d",
            local->name, lineno);
    }
    global->status = local->status;
    global->segment = local->segment;
    global->value = local->value;
    while (local->fixups != NULL) {
      f = local->fixups;
      local->fixups = f->next;
      f->next = global->fixups;
      global->fixups = f;
    }
    local->status = STATUS_GLOBREF;
    local->globref = global;
    getToken();
    if (token != TOK_COMMA) {
      break;
    }
    getToken();
  }
}


void dotImport(unsigned int code) {
  Symbol *global;
  Symbol *local;
  Fixup *f;

  while (1) {
    expect(TOK_IDENT);
    global = lookupEnter(tokenvalString, GLOBAL_TABLE);
    local = lookupEnter(tokenvalString, LOCAL_TABLE);
    if (local->status != STATUS_UNKNOWN) {
      error("imported symbol '%s' multiply defined in line %d",
            local->name, lineno);
    }
    while (local->fixups != NULL) {
      f = local->fixups;
      local->fixups = f->next;
      f->next = global->fixups;
      global->fixups = f;
    }
    local->status = STATUS_GLOBREF;
    local->globref = global;
    getToken();
    if (token != TOK_COMMA) {
      break;
    }
    getToken();
  }
}


int countBits(unsigned int x) {
  int n;

  n = 0;
  while (x != 0) {
    x &= x - 1;
    n++;
  }
  return n;
}


void dotAlign(unsigned int code) {
  Value v;
  unsigned int mask;

  v = parseExpression();
  if (v.sym != NULL) {
    error("absolute expression expected in line %d", lineno);
  }
  if (countBits(v.con) != 1) {
    error("argument must be a power of 2 in line %d", lineno);
  }
  mask = v.con - 1;
  while ((segPtr[currSeg] & mask) != 0) {
    emitByte(0);
  }
}


void dotSpace(unsigned int code) {
  Value v;
  int i;

  v = parseExpression();
  if (v.sym != NULL) {
    error("absolute expression expected in line %d", lineno);
  }
  for (i = 0; i < v.con; i++) {
    emitByte(0);
  }
}


void dotLocate(unsigned int code) {
  Value v;

  v = parseExpression();
  if (v.sym != NULL) {
    error("absolute expression expected in line %d", lineno);
  }
  while (segPtr[currSeg] != v.con) {
    emitByte(0);
  }
}


void dotByte(unsigned int code) {
  Value v;
  char *p;

  while (1) {
    if (token == TOK_STRING) {
      p = tokenvalString;
      while (*p != '\0') {
        emitByte(*p);
        p++;
      }
      getToken();
    } else {
      v = parseExpression();
      if (v.sym != NULL) {
        error("absolute expression expected in line %d", lineno);
      }
      emitByte(v.con);
    }
    if (token != TOK_COMMA) {
      break;
    }
    getToken();
  }
}


void dotHalf(unsigned int code) {
  Value v;

  while (1) {
    v = parseExpression();
    if (v.sym != NULL) {
      error("absolute expression expected in line %d", lineno);
    }
    emitHalf(v.con);
    if (token != TOK_COMMA) {
      break;
    }
    getToken();
  }
}


void dotWord(unsigned int code) {
  Value v;

  while (1) {
    v = parseExpression();
    if (v.sym == NULL) {
      emitWord(v.con);
    } else {
      addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_W32, v.con);
      emitWord(0);
    }
    if (token != TOK_COMMA) {
      break;
    }
    getToken();
  }
}


void dotSet(unsigned int code) {
  Value v;
  Symbol *symbol;

  expect(TOK_IDENT);
  symbol = deref(lookupEnter(tokenvalString, LOCAL_TABLE));
  if (symbol->status != STATUS_UNKNOWN) {
    error("symbol '%s' multiply defined in line %d",
          symbol->name, lineno);
  }
  getToken();
  expect(TOK_COMMA);
  getToken();
  v = parseExpression();
  if (v.sym == NULL) {
    symbol->status = STATUS_DEFINED;
    symbol->segment = SEGMENT_ABS;
    symbol->value = v.con;
  } else {
    error("illegal type of symbol '%s' in expression, line %d",
          v.sym->name, lineno);
  }
}


/*
 * .ldgot <GOT reg>
 * emit instructions to get address of GOT in a reg
 */
void dotLdgot(unsigned int code) {
  if (!genPIC) {
    error("illegal .ldgot in line %d, cmdline switch -p missing?", lineno);
  }
  expect(TOK_REGISTER);
  gotReg = tokenvalNumber;
  getToken();
  emitWord(OP_JAL << 26);
  addFixupToLst(currSeg, segPtr[currSeg], RELOC_GOTADRH, 0);
  emitWord(OP_LDHI << 26 | gotReg << 16);
  addFixupToLst(currSeg, segPtr[currSeg], RELOC_GOTADRL, 4);
  emitWord(OP_ORI << 26 | gotReg << 21 | gotReg << 16);
  emitWord(OP_ADD << 26 | gotReg << 21 | 31 << 16 | gotReg << 11);
}


void formatN(unsigned int code) {
  Value v;
  unsigned int immed;

  /* opcode with no operands */
  if (token != TOK_EOL) {
    /* in exceptional cases (trap) there may be one constant operand */
    v = parseExpression();
    if (v.sym != NULL) {
      error("operand must be a constant, line %d", lineno);
    }
    immed = v.con;
  } else {
    immed = 0;
  }
  emitWord(code << 26 | (immed & 0x03FFFFFF));
}


void formatRH(unsigned int code) {
  int reg;
  Value v;

  /* opcode with one register and a half operand */
  expect(TOK_REGISTER);
  reg = tokenvalNumber;
  getToken();
  expect(TOK_COMMA);
  getToken();
  v = parseExpression();
  if (v.sym == NULL) {
    emitHalf(code << 10 | reg);
    emitHalf(v.con);
  } else {
    addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_L16, v.con);
    emitHalf(code << 10 | reg);
    emitHalf(0);
  }
}


void formatRHH(unsigned int code) {
  int reg;
  Value v;

  /* opcode with one register and a half operand */
  /* ATTENTION: high order 16 bits encoded in instruction */
  expect(TOK_REGISTER);
  reg = tokenvalNumber;
  getToken();
  expect(TOK_COMMA);
  getToken();
  v = parseExpression();
  if (v.sym == NULL) {
    emitHalf(code << 10 | reg);
    emitHalf(v.con >> 16);
  } else {
    addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_H16, v.con);
    emitHalf(code << 10 | reg);
    emitHalf(0);
  }
}


void formatRRS(unsigned int code) {
  int dst, src;
  Value v;

  /* opcode with two registers and a signed half operand */
  expect(TOK_REGISTER);
  dst = tokenvalNumber;
  getToken();
  expect(TOK_COMMA);
  getToken();
  expect(TOK_REGISTER);
  src = tokenvalNumber;
  getToken();
  expect(TOK_COMMA);
  getToken();
  v = parseExpression();
  if (allowSyn) {
    if (v.sym == NULL) {
      if ((v.con & 0xFFFF8000) == 0x00000000 ||
          (v.con & 0xFFFF8000) == 0xFFFF8000) {
        /* code: op dst,src,con */
        emitHalf(code << 10 | src << 5 | dst);
        emitHalf(v.con);
      } else {
        /* code: ldhi $1,con; or $1,$1,con; add $1,$1,src; op dst,$1,0 */
        emitHalf(OP_LDHI << 10 | AUX_REG);
        emitHalf(v.con >> 16);
        emitHalf((OP_OR + 1) << 10 | AUX_REG << 5 | AUX_REG);
        emitHalf(v.con);
        emitHalf(OP_ADD << 10 | AUX_REG << 5 | src);
        emitHalf(AUX_REG << 11);
        emitHalf(code << 10 | AUX_REG << 5 | dst);
        emitHalf(0);
      }
    } else {
      if (genPIC) {
        if (v.sym->whichTable == LOCAL_TABLE) {
          /* GOT-relative access */
          addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_GOTOFFH, 0);
          emitWord(OP_LDHI << 26 | AUX_REG << 16);
          addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_GOTOFFL, 0);
          emitWord(OP_ORI << 26 | AUX_REG << 21 | AUX_REG << 16);
          emitWord(OP_ADD << 26 | AUX_REG << 21 | gotReg << 16 | AUX_REG << 11);
        } else {
          /* GOT-indirect access */
          addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_GOTPNTR, 0);
          emitWord(OP_LDW << 26 | gotReg << 21 | AUX_REG << 16);
        }
        emitHalf(code << 10 | AUX_REG << 5 | dst);
        emitHalf(v.con);
      } else {
        /* code: ldhi $1,con; or $1,$1,con; add $1,$1,src; op dst,$1,0 */
        addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_H16, v.con);
        emitHalf(OP_LDHI << 10 | AUX_REG);
        emitHalf(0);
        addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_L16, v.con);
        emitHalf((OP_OR + 1) << 10 | AUX_REG << 5 | AUX_REG);
        emitHalf(0);
        emitHalf(OP_ADD << 10 | AUX_REG << 5 | src);
        emitHalf(AUX_REG << 11);
        emitHalf(code << 10 | AUX_REG << 5 | dst);
        emitHalf(0);
      }
    }
  } else {
    if (v.sym == NULL) {
      emitHalf(code << 10 | src << 5 | dst);
      emitHalf(v.con);
    } else {
      addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_L16, v.con);
      emitHalf(code << 10 | src << 5 | dst);
      emitHalf(0);
    }
  }
}


void formatRRX(unsigned int code) {
  int dst, src1, src2;
  Value v;

  /* opcode with three register operands
     or two registers and a half operand */
  expect(TOK_REGISTER);
  dst = tokenvalNumber;
  getToken();
  expect(TOK_COMMA);
  getToken();
  expect(TOK_REGISTER);
  src1 = tokenvalNumber;
  getToken();
  expect(TOK_COMMA);
  getToken();
  if (token == TOK_REGISTER) {
    src2 = tokenvalNumber;
    getToken();
    emitHalf(code << 10 | src1 << 5 | src2);
    emitHalf(dst << 11);
  } else {
    v = parseExpression();
    if (allowSyn) {
      if (v.sym == NULL) {
        if ((v.con & 0xFFFF0000) == 0) {
          /* code: op dst,src,con */
          emitHalf((code + 1) << 10 | src1 << 5 | dst);
          emitHalf(v.con);
        } else {
          if ((v.con & 0x0000FFFF) == 0) {
            /* code: ldhi $1,con; op dst,src,$1 */
            emitHalf(OP_LDHI << 10 | AUX_REG);
            emitHalf(v.con >> 16);
            emitHalf(code << 10 | src1 << 5 | AUX_REG);
            emitHalf(dst << 11);
          } else {
            /* code: ldhi $1,con; or $1,$1,con; op dst,src,$1 */
            emitHalf(OP_LDHI << 10 | AUX_REG);
            emitHalf(v.con >> 16);
            emitHalf((OP_OR + 1) << 10 | AUX_REG << 5 | AUX_REG);
            emitHalf(v.con);
            emitHalf(code << 10 | src1 << 5 | AUX_REG);
            emitHalf(dst << 11);
          }
        }
      } else {
        if (genPIC) {
          if (v.sym->whichTable == LOCAL_TABLE) {
            /* GOT-relative access */
            addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_GOTOFFH, 0);
            emitWord(OP_LDHI << 26 | AUX_REG << 16);
            addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_GOTOFFL, 0);
            emitWord(OP_ORI << 26 | AUX_REG << 21 | AUX_REG << 16);
            emitWord(OP_ADD << 26 | AUX_REG << 21 | gotReg << 16 | AUX_REG << 11);
          } else {
            /* GOT-indirect access */
            addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_GOTPNTR, 0);
            emitWord(OP_LDW << 26 | gotReg << 21 | AUX_REG << 16);
          }
          emitHalf(code << 10 | src1 << 5 | AUX_REG);
          emitHalf(dst << 11);
        } else {
          /* code: ldhi $1,con; or $1,$1,con; op dst,src,$1 */
          addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_H16, v.con);
          emitHalf(OP_LDHI << 10 | AUX_REG);
          emitHalf(0);
          addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_L16, v.con);
          emitHalf((OP_OR + 1) << 10 | AUX_REG << 5 | AUX_REG);
          emitHalf(0);
          emitHalf(code << 10 | src1 << 5 | AUX_REG);
          emitHalf(dst << 11);
        }
      }
    } else {
      if (v.sym == NULL) {
        emitHalf((code + 1) << 10 | src1 << 5 | dst);
        emitHalf(v.con);
      } else {
        addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_L16, v.con);
        emitHalf((code + 1) << 10 | src1 << 5 | dst);
        emitHalf(0);
      }
    }
  }
}


void formatRRY(unsigned int code) {
  int dst, src1, src2;
  Value v;

  /* opcode with three register operands
     or two registers and a signed half operand */
  expect(TOK_REGISTER);
  dst = tokenvalNumber;
  getToken();
  expect(TOK_COMMA);
  getToken();
  expect(TOK_REGISTER);
  src1 = tokenvalNumber;
  getToken();
  expect(TOK_COMMA);
  getToken();
  if (token == TOK_REGISTER) {
    src2 = tokenvalNumber;
    getToken();
    emitHalf(code << 10 | src1 << 5 | src2);
    emitHalf(dst << 11);
  } else {
    v = parseExpression();
    if (allowSyn) {
      if (v.sym == NULL) {
        if ((v.con & 0xFFFF8000) == 0x00000000 ||
            (v.con & 0xFFFF8000) == 0xFFFF8000) {
          /* code: op dst,src,con */
          emitHalf((code + 1) << 10 | src1 << 5 | dst);
          emitHalf(v.con);
        } else {
          if ((v.con & 0x0000FFFF) == 0) {
            /* code: ldhi $1,con; op dst,src,$1 */
            emitHalf(OP_LDHI << 10 | AUX_REG);
            emitHalf(v.con >> 16);
            emitHalf(code << 10 | src1 << 5 | AUX_REG);
            emitHalf(dst << 11);
          } else {
            /* code: ldhi $1,con; or $1,$1,con; op dst,src,$1 */
            emitHalf(OP_LDHI << 10 | AUX_REG);
            emitHalf(v.con >> 16);
            emitHalf((OP_OR + 1) << 10 | AUX_REG << 5 | AUX_REG);
            emitHalf(v.con);
            emitHalf(code << 10 | src1 << 5 | AUX_REG);
            emitHalf(dst << 11);
          }
        }
      } else {
        if (genPIC) {
          if (v.sym->whichTable == LOCAL_TABLE) {
            /* GOT-relative access */
            addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_GOTOFFH, 0);
            emitWord(OP_LDHI << 26 | AUX_REG << 16);
            addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_GOTOFFL, 0);
            emitWord(OP_ORI << 26 | AUX_REG << 21 | AUX_REG << 16);
            emitWord(OP_ADD << 26 | AUX_REG << 21 | gotReg << 16 | AUX_REG << 11);
          } else {
            /* GOT-indirect access */
            addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_GOTPNTR, 0);
            emitWord(OP_LDW << 26 | gotReg << 21 | AUX_REG << 16);
          }
          emitHalf(code << 10 | src1 << 5 | AUX_REG);
          emitHalf(dst << 11);
        } else {
          /* code: ldhi $1,con; or $1,$1,con; op dst,src,$1 */
          addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_H16, v.con);
          emitHalf(OP_LDHI << 10 | AUX_REG);
          emitHalf(0);
          addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_L16, v.con);
          emitHalf((OP_OR + 1) << 10 | AUX_REG << 5 | AUX_REG);
          emitHalf(0);
          emitHalf(code << 10 | src1 << 5 | AUX_REG);
          emitHalf(dst << 11);
        }
      }
    } else {
      if (v.sym == NULL) {
        emitHalf((code + 1) << 10 | src1 << 5 | dst);
        emitHalf(v.con);
      } else {
        addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_L16, v.con);
        emitHalf((code + 1) << 10 | src1 << 5 | dst);
        emitHalf(0);
      }
    }
  }
}


void formatRRB(unsigned int code) {
  int src1, src2;
  Value v;
  unsigned int immed;

  /* opcode with two registers and a 16 bit signed offset operand */
  expect(TOK_REGISTER);
  src1 = tokenvalNumber;
  getToken();
  expect(TOK_COMMA);
  getToken();
  expect(TOK_REGISTER);
  src2 = tokenvalNumber;
  getToken();
  expect(TOK_COMMA);
  getToken();
  v = parseExpression();
  if (v.sym == NULL) {
    immed = (v.con - ((signed) segPtr[currSeg] + 4)) / 4;
  } else {
    addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_R16, v.con);
    immed = 0;
  }
  emitHalf(code << 10 | src1 << 5 | src2);
  emitHalf(immed);
}


void formatJ(unsigned int code) {
  Value v;
  unsigned int immed;
  int target;

  /* opcode with no registers and a 26 bit signed offset operand or
     opcode with a single register */
  if (token == TOK_REGISTER) {
    target = tokenvalNumber;
    getToken();
    emitWord((code + 1) << 26 | target << 21);
  } else {
    v = parseExpression();
    if (v.sym == NULL) {
      immed = (v.con - ((signed) segPtr[currSeg] + 4)) / 4;
      emitWord(code << 26 | (immed & 0x03FFFFFF));
    } else {
      if (genPIC) {
        if (v.sym->whichTable == LOCAL_TABLE) {
          /* generate PC-relative jump with R26 relocation */
          addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_R26, v.con);
          immed = 0;
          emitWord(code << 26 | (immed & 0x03FFFFFF));
        } else {
          /* load address from GOT in register and jump */
          addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_GOTPNTR, 0);
          emitWord(OP_LDW << 26 | gotReg << 21 | AUX_REG << 16);
          emitWord((code + 1) << 26 | AUX_REG << 21);
        }
      } else {
        addFixupToSym(v.sym, currSeg, segPtr[currSeg], RELOC_R26, v.con);
        immed = 0;
        emitWord(code << 26 | (immed & 0x03FFFFFF));
      }
    }
  }
}


void formatJR(unsigned int code) {
  int target;

  /* opcode with one register operand */
  expect(TOK_REGISTER);
  target = tokenvalNumber;
  getToken();
  emitWord(code << 26 | target << 21);
}


typedef struct instr {
  char *name;
  void (*func)(unsigned int code);
  unsigned int code;
} Instr;


Instr instrTable[] = {

  /* pseudo instructions */
  { ".syn",    dotSyn,    0 },
  { ".nosyn",  dotNosyn,  0 },
  { ".code",   dotCode,   0 },
  { ".data",   dotData,   0 },
  { ".bss",    dotBss,    0 },
  { ".export", dotExport, 0 },
  { ".import", dotImport, 0 },
  { ".align",  dotAlign,  0 },
  { ".space",  dotSpace,  0 },
  { ".locate", dotLocate, 0 },
  { ".byte",   dotByte,   0 },
  { ".half",   dotHalf,   0 },
  { ".word",   dotWord,   0 },
  { ".set",    dotSet,    0 },
  { ".ldgot",  dotLdgot,  0 },

  /* arithmetical instructions */
  { "add",     formatRRY, OP_ADD  },
  { "sub",     formatRRY, OP_SUB  },

  { "mul",     formatRRY, OP_MUL  },
  { "mulu",    formatRRX, OP_MULU },
  { "div",     formatRRY, OP_DIV  },
  { "divu",    formatRRX, OP_DIVU },
  { "rem",     formatRRY, OP_REM  },
  { "remu",    formatRRX, OP_REMU },

  /* logical instructions */
  { "and",     formatRRX, OP_AND  },
  { "or",      formatRRX, OP_OR   },
  { "xor",     formatRRX, OP_XOR  },
  { "xnor",    formatRRX, OP_XNOR },

  /* shift instructions */
  { "sll",     formatRRX, OP_SLL  },
  { "slr",     formatRRX, OP_SLR  },
  { "sar",     formatRRX, OP_SAR  },

  /* cache control instructions */
  { "cctl",    formatN,   OP_CCTL },

  /* load immediate instructions */
  { "ldhi",    formatRHH, OP_LDHI },

  /* branch instructions */
  { "beq",     formatRRB, OP_BEQ  },
  { "bne",     formatRRB, OP_BNE  },
  { "ble",     formatRRB, OP_BLE  },
  { "bleu",    formatRRB, OP_BLEU },
  { "blt",     formatRRB, OP_BLT  },
  { "bltu",    formatRRB, OP_BLTU },
  { "bge",     formatRRB, OP_BGE  },
  { "bgeu",    formatRRB, OP_BGEU },
  { "bgt",     formatRRB, OP_BGT  },
  { "bgtu",    formatRRB, OP_BGTU },

  /* jump, call & return instructions */
  { "j",       formatJ,   OP_J    },
  { "jr",      formatJR,  OP_JR   },
  { "jal",     formatJ,   OP_JAL  },
  { "jalr",    formatJR,  OP_JALR },

  /* interrupt related instructions */
  { "trap",    formatN,   OP_TRAP },
  { "rfx",     formatN,   OP_RFX  },

  /* load instructions */
  { "ldw",     formatRRS, OP_LDW  },
  { "ldh",     formatRRS, OP_LDH  },
  { "ldhu",    formatRRS, OP_LDHU },
  { "ldb",     formatRRS, OP_LDB  },
  { "ldbu",    formatRRS, OP_LDBU },

  /* store instructions */
  { "stw",     formatRRS, OP_STW  },
  { "sth",     formatRRS, OP_STH  },
  { "stb",     formatRRS, OP_STB  },

  /* processor control instructions */
  { "mvfs",    formatRH,  OP_MVFS },
  { "mvts",    formatRH,  OP_MVTS },
  { "tbs",     formatN,   OP_TBS  },
  { "tbwr",    formatN,   OP_TBWR },
  { "tbri",    formatN,   OP_TBRI },
  { "tbwi",    formatN,   OP_TBWI },

  /* synchronization instructions */
  { "ldlw",    formatRRS, OP_LDLW },
  { "stcw",    formatRRS, OP_STCW },

};


static int cmpInstr(const void *instr1, const void *instr2) {
  return strcmp(((Instr *) instr1)->name, ((Instr *) instr2)->name);
}


void sortInstrTable(void) {
  qsort(instrTable, sizeof(instrTable)/sizeof(instrTable[0]),
        sizeof(instrTable[0]), cmpInstr);
}


Instr *lookupInstr(char *name) {
  int lo, hi, tst;
  int res;

  lo = 0;
  hi = sizeof(instrTable) / sizeof(instrTable[0]) - 1;
  while (lo <= hi) {
    tst = (lo + hi) / 2;
    res = strcmp(instrTable[tst].name, name);
    if (res == 0) {
      return &instrTable[tst];
    }
    if (res < 0) {
      lo = tst + 1;
    } else {
      hi = tst - 1;
    }
  }
  return NULL;
}


/**************************************************************/


void asmModule(void) {
  Symbol *label;
  Instr *instr;

  /* extra pass to handle imported/exported symbols */
  allowSyn = 1;
  currSeg = SEGMENT_CODE;
  lineno = 0;
  while (fgets(line, LINE_SIZE, inFile) != NULL) {
    lineno++;
    lineptr = line;
    getToken();
    while (token == TOK_LABEL) {
      getToken();
    }
    if (token == TOK_IDENT) {
      instr = lookupInstr(tokenvalString);
      if (instr == NULL) {
        error("unknown instruction '%s' in line %d",
              tokenvalString, lineno);
      }
      getToken();
      if (instr->func == dotExport ||
          instr->func == dotImport) {
        /* handle */
        (*instr->func)(instr->code);
      } else {
        /* skip */
        while (token != TOK_EOL) {
          getToken();
        }
      }
    }
    if (token != TOK_EOL) {
      error("garbage in line %d", lineno);
    }
  }
  rewind(inFile);
  /* regular pass to assemble the module */
  allowSyn = 1;
  currSeg = SEGMENT_CODE;
  lineno = 0;
  while (fgets(line, LINE_SIZE, inFile) != NULL) {
    lineno++;
    lineptr = line;
    getToken();
    while (token == TOK_LABEL) {
      label = deref(lookupEnter(tokenvalString, LOCAL_TABLE));
      if (label->status != STATUS_UNKNOWN) {
        error("label '%s' multiply defined in line %d",
              label->name, lineno);
      }
      label->status = STATUS_DEFINED;
      label->segment = currSeg;
      label->value = segPtr[currSeg];
      getToken();
    }
    if (token == TOK_IDENT) {
      instr = lookupInstr(tokenvalString);
      if (instr == NULL) {
        error("unknown instruction '%s' in line %d",
              tokenvalString, lineno);
      }
      getToken();
      if (instr->func == dotExport ||
          instr->func == dotImport) {
        /* skip */
        while (token != TOK_EOL) {
          getToken();
        }
      } else {
        /* handle */
        (*instr->func)(instr->code);
      }
    }
    if (token != TOK_EOL) {
      error("garbage in line %d", lineno);
    }
  }
}


/**************************************************************/


unsigned int read4FromEco(unsigned char *p) {
  return (unsigned int) p[0] << 24 |
         (unsigned int) p[1] << 16 |
         (unsigned int) p[2] <<  8 |
         (unsigned int) p[3] <<  0;
}


void write4ToEco(unsigned char *p, unsigned int data) {
  p[0] = data >> 24;
  p[1] = data >> 16;
  p[2] = data >>  8;
  p[3] = data >>  0;
}


void conv4FromEcoToNative(unsigned char *p) {
  unsigned int data;

  data = read4FromEco(p);
  * (unsigned int *) p = data;
}


void conv4FromNativeToEco(unsigned char *p) {
  unsigned int data;

  data = * (unsigned int *) p;
  write4ToEco(p, data);
}


/**************************************************************/


#define CODE_NAME	".code"
#define DATA_NAME	".data"
#define BSS_NAME	".bss"


static ExecHeader execHeader;

static int fileOffset;
static int dataSize;
static int stringSize;

static int nsyms;
static int nrels;


static void writeDummyHeader(void) {
  fwrite(&execHeader, sizeof(ExecHeader), 1, outFile);
  /* update file offset */
  fileOffset += sizeof(ExecHeader);
}


static void writeRealHeader(void) {
  rewind(outFile);
  execHeader.magic = EXEC_MAGIC;
  execHeader.entry = 0;
  conv4FromNativeToEco((unsigned char *) &execHeader.magic);
  conv4FromNativeToEco((unsigned char *) &execHeader.osegs);
  conv4FromNativeToEco((unsigned char *) &execHeader.nsegs);
  conv4FromNativeToEco((unsigned char *) &execHeader.osyms);
  conv4FromNativeToEco((unsigned char *) &execHeader.nsyms);
  conv4FromNativeToEco((unsigned char *) &execHeader.orels);
  conv4FromNativeToEco((unsigned char *) &execHeader.nrels);
  conv4FromNativeToEco((unsigned char *) &execHeader.odata);
  conv4FromNativeToEco((unsigned char *) &execHeader.sdata);
  conv4FromNativeToEco((unsigned char *) &execHeader.ostrs);
  conv4FromNativeToEco((unsigned char *) &execHeader.sstrs);
  conv4FromNativeToEco((unsigned char *) &execHeader.entry);
  fwrite(&execHeader, sizeof(ExecHeader), 1, outFile);
  conv4FromEcoToNative((unsigned char *) &execHeader.magic);
  conv4FromEcoToNative((unsigned char *) &execHeader.osegs);
  conv4FromEcoToNative((unsigned char *) &execHeader.nsegs);
  conv4FromEcoToNative((unsigned char *) &execHeader.osyms);
  conv4FromEcoToNative((unsigned char *) &execHeader.nsyms);
  conv4FromEcoToNative((unsigned char *) &execHeader.orels);
  conv4FromEcoToNative((unsigned char *) &execHeader.nrels);
  conv4FromEcoToNative((unsigned char *) &execHeader.odata);
  conv4FromEcoToNative((unsigned char *) &execHeader.sdata);
  conv4FromEcoToNative((unsigned char *) &execHeader.ostrs);
  conv4FromEcoToNative((unsigned char *) &execHeader.sstrs);
  conv4FromEcoToNative((unsigned char *) &execHeader.entry);
}


static void writeSegment(SegmentRecord *p) {
  conv4FromNativeToEco((unsigned char *) &p->name);
  conv4FromNativeToEco((unsigned char *) &p->offs);
  conv4FromNativeToEco((unsigned char *) &p->addr);
  conv4FromNativeToEco((unsigned char *) &p->size);
  conv4FromNativeToEco((unsigned char *) &p->attr);
  fwrite(p, sizeof(SegmentRecord), 1, outFile);
  conv4FromEcoToNative((unsigned char *) &p->name);
  conv4FromEcoToNative((unsigned char *) &p->offs);
  conv4FromEcoToNative((unsigned char *) &p->addr);
  conv4FromEcoToNative((unsigned char *) &p->size);
  conv4FromEcoToNative((unsigned char *) &p->attr);
}


static void writeSegmentTable(void) {
  SegmentRecord segment;

  /* record file offset */
  execHeader.osegs = fileOffset;
  /* write code segment descriptor */
  segment.name = stringSize;
  stringSize += strlen(CODE_NAME) + 1;
  segment.offs = dataSize;
  segment.addr = 0;
  segment.size = segPtr[SEGMENT_CODE];
  segment.attr = SEG_ATTR_A | SEG_ATTR_P | SEG_ATTR_X;
  writeSegment(&segment);
  dataSize += segment.size;
  /* write data segment descriptor */
  segment.name = stringSize;
  stringSize += strlen(DATA_NAME) + 1;
  segment.offs = dataSize;
  segment.addr = 0;
  segment.size = segPtr[SEGMENT_DATA];
  segment.attr = SEG_ATTR_A | SEG_ATTR_P | SEG_ATTR_W;
  writeSegment(&segment);
  dataSize += segment.size;
  /* write bss segment descriptor */
  segment.name = stringSize;
  stringSize += strlen(BSS_NAME) + 1;
  segment.offs = dataSize;
  segment.addr = 0;
  segment.size = segPtr[SEGMENT_BSS];
  segment.attr = SEG_ATTR_A | SEG_ATTR_W;
  writeSegment(&segment);
  dataSize += 0;  /* segment not present */
  /* update file offset */
  execHeader.nsegs = 3;
  fileOffset += execHeader.nsegs * sizeof(SegmentRecord);
}


static void writeSymbol(Symbol *s) {
  SymbolRecord symRec;

  if (s->skip) {
    /* this symbol is neither defined here nor referenced here: skip */
    return;
  }
  symRec.name = stringSize;
  stringSize += strlen(s->name) + 1;
  if (s->status == STATUS_UNKNOWN) {
    symRec.val = 0;
    symRec.seg = -1;
    symRec.attr = SYM_ATTR_U;
  } else {
    symRec.val = s->value;
    /* change (.abs, .code, .data, .bss) from (0..3) to (-1..2) */
    symRec.seg = s->segment - 1;
    symRec.attr = 0;
  }
  conv4FromNativeToEco((unsigned char *) &symRec.name);
  conv4FromNativeToEco((unsigned char *) &symRec.val);
  conv4FromNativeToEco((unsigned char *) &symRec.seg);
  conv4FromNativeToEco((unsigned char *) &symRec.attr);
  fwrite(&symRec, sizeof(SymbolRecord), 1, outFile);
  conv4FromEcoToNative((unsigned char *) &symRec.name);
  conv4FromEcoToNative((unsigned char *) &symRec.val);
  conv4FromEcoToNative((unsigned char *) &symRec.seg);
  conv4FromEcoToNative((unsigned char *) &symRec.attr);
  nsyms++;
}


static void writeSymbolTable(void) {
  /* record file offset */
  execHeader.osyms = fileOffset;
  /* write symbol table */
  nsyms = 0;
  walkTree(globalTable, writeSymbol);
  /* update file offset */
  execHeader.nsyms = nsyms;
  fileOffset += execHeader.nsyms * sizeof(SymbolRecord);
}


static void writeRelocTable(void) {
  Fixup *f;
  RelocRecord relRec;

  /* record file offset */
  execHeader.orels = fileOffset;
  /* write reloc table */
  nrels = 0;
  f = fixupList;
  while (f != NULL) {
    if (f->segment != SEGMENT_CODE && f->segment != SEGMENT_DATA) {
      /* this should never happen */
      error("fixup found in a segment other than code or data");
    }
    relRec.loc = f->offset;
    /* change (.code, .data) from (1, 2) to (0, 1) */
    relRec.seg = f->segment - 1;
    relRec.typ = f->method;
    /* 'typ' knows if a symbol or a segment is referenced */
    if (f->base & MSB) {
      /* it's a symbol */
      relRec.typ |= RELOC_SYM;
    }
    /* but 'ref' is a plain number */
    if (f->base & MSB) {
      /* so clear MSB in case of symbol reference */
      relRec.ref = f->base & ~MSB;
    } else {
      /* and correct segment otherwise, see above */
      relRec.ref = f->base - 1;
    }
    relRec.add = f->value;
    conv4FromNativeToEco((unsigned char *) &relRec.loc);
    conv4FromNativeToEco((unsigned char *) &relRec.seg);
    conv4FromNativeToEco((unsigned char *) &relRec.typ);
    conv4FromNativeToEco((unsigned char *) &relRec.ref);
    conv4FromNativeToEco((unsigned char *) &relRec.add);
    fwrite(&relRec, sizeof(RelocRecord), 1, outFile);
    conv4FromEcoToNative((unsigned char *) &relRec.loc);
    conv4FromEcoToNative((unsigned char *) &relRec.seg);
    conv4FromEcoToNative((unsigned char *) &relRec.typ);
    conv4FromEcoToNative((unsigned char *) &relRec.ref);
    conv4FromEcoToNative((unsigned char *) &relRec.add);
    nrels++;
    f = f->next;
  }
  /* update file offset */
  execHeader.nrels = nrels;
  fileOffset += execHeader.nrels * sizeof(RelocRecord);
}


static void writeBytes(FILE *file) {
  int data;

  rewind(file);
  while (1) {
    data = fgetc(file);
    if (data == EOF) {
      break;
    }
    fputc(data, outFile);
  }
}


static void writeData(void) {
  /* record file offset */
  execHeader.odata = fileOffset;
  /* write segment data */
  writeBytes(codeFile);
  writeBytes(dataFile);
  /* update file offset */
  execHeader.sdata = dataSize;
  fileOffset += execHeader.sdata;
}


static void writeString(Symbol *s) {
  if (s->skip) {
    /* this symbol is neither defined here nor referenced here: skip */
    return;
  }
  fputs(s->name, outFile);
  fputc('\0', outFile);
}


static void writeStrings(void) {
  /* record file offset */
  execHeader.ostrs = fileOffset;
  /* write segment names */
  fputs(CODE_NAME, outFile);
  fputc('\0', outFile);
  fputs(DATA_NAME, outFile);
  fputc('\0', outFile);
  fputs(BSS_NAME, outFile);
  fputc('\0', outFile);
  /* write symbol names */
  walkTree(globalTable, writeString);
  /* update file offsets */
  execHeader.sstrs = stringSize;
  fileOffset += execHeader.sstrs;
}


void writeAll(void) {
  fileOffset = 0;
  dataSize = 0;
  stringSize = 0;
  writeDummyHeader();
  writeSegmentTable();
  writeSymbolTable();
  writeRelocTable();
  writeData();
  writeStrings();
  writeRealHeader();
}


/**************************************************************/


void usage(char *myself) {
  fprintf(stderr, "Usage: %s\n", myself);
  fprintf(stderr, "         [-p]             position-independent code\n");
  fprintf(stderr, "         [-o objfile]     set object file name\n");
  fprintf(stderr, "         file             source file name\n");
  fprintf(stderr, "         [files...]       additional source files\n");
  exit(1);
}


int main(int argc, char *argv[]) {
  int i;
  char *argp;

  if (debugCmdline) {
    for (i = 1; i < argc; i++) {
      fprintf(stderr, "%s cmdline arg %d: %s\n", argv[0], i, argv[i]);
    }
  }
  sortInstrTable();
  tmpnam(codeName);
  tmpnam(dataName);
  outName = "a.out";
  genPIC = 0;
  for (i = 1; i < argc; i++) {
    argp = argv[i];
    if (*argp != '-') {
      break;
    }
    argp++;
    switch (*argp) {
      case 'o':
        if (i == argc - 1) {
          usage(argv[0]);
        }
        outName = argv[++i];
        break;
      case 'p':
        genPIC = 1;
        break;
      default:
        usage(argv[0]);
    }
  }
  if (i == argc) {
    usage(argv[0]);
  }
  codeFile = fopen(codeName, "w+b");
  if (codeFile == NULL) {
    error("cannot create temporary code file '%s'", codeName);
  }
  dataFile = fopen(dataName, "w+b");
  if (dataFile == NULL) {
    error("cannot create temporary data file '%s'", dataName);
  }
  outFile = fopen(outName, "wb");
  if (outFile == NULL) {
    error("cannot open output file '%s'", outName);
  }
  do {
    inName = argv[i];
    if (*inName == '-') {
      usage(argv[0]);
    }
    inFile = fopen(inName, "rt");
    if (inFile == NULL) {
      error("cannot open input file '%s'", inName);
    }
    if (debugModule) {
      fprintf(stderr, "Assembling module '%s'...\n", inName);
    }
    asmModule();
    if (inFile != NULL) {
      fclose(inFile);
      inFile = NULL;
    }
    linkLocals();
  } while (++i < argc);
  transferFixups();
  writeAll();
  if (codeFile != NULL) {
    fclose(codeFile);
    codeFile = NULL;
  }
  if (dataFile != NULL) {
    fclose(dataFile);
    dataFile = NULL;
  }
  if (outFile != NULL) {
    fclose(outFile);
    outFile = NULL;
  }
  if (codeName != NULL) {
    unlink(codeName);
  }
  if (dataName != NULL) {
    unlink(dataName);
  }
  return 0;
}
