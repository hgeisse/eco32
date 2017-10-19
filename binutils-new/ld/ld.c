/*
 * ld.c -- ECO32 linking loader
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../include/a.out.h"
#include "utils.h"
#include "readscript.h"


#define DEFAULT_SCRIPT_NAME	"standard.lnk"
#define DEFAULT_OUT_NAME	"a.out"

#define WORD_ALIGN(x)		(((x) + 0x03) & ~0x03)


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


typedef struct module {
  char *name;
  char *strs;
  int nsegs;
  SegmentRecord *segtbl;
  struct module *next;
} Module;


Module *allModules = NULL;
Module *lastModule;


Module *newModule(char *name) {
  Module *mp;

  mp = memAlloc(sizeof(Module));
  mp->name = name;
  mp->strs = NULL;
  mp->nsegs = 0;
  mp->segtbl = NULL;
  mp->next = NULL;
  if (allModules == NULL) {
    allModules = mp;
  } else {
    lastModule->next = mp;
  }
  lastModule = mp;
  return mp;
}


/**************************************************************/


void readHeader(ExecHeader *hp, FILE *inFile, char *inName) {
  if (fseek(inFile, 0, SEEK_SET) < 0) {
    error("cannot seek to header in input file '%s'", inName);
  }
  if (fread(hp, sizeof(ExecHeader), 1, inFile) != 1) {
    error("cannot read header in input file '%s'", inName);
  }
  conv4FromEcoToNative((unsigned char *) &hp->magic);
  conv4FromEcoToNative((unsigned char *) &hp->osegs);
  conv4FromEcoToNative((unsigned char *) &hp->nsegs);
  conv4FromEcoToNative((unsigned char *) &hp->osyms);
  conv4FromEcoToNative((unsigned char *) &hp->nsyms);
  conv4FromEcoToNative((unsigned char *) &hp->orels);
  conv4FromEcoToNative((unsigned char *) &hp->nrels);
  conv4FromEcoToNative((unsigned char *) &hp->odata);
  conv4FromEcoToNative((unsigned char *) &hp->sdata);
  conv4FromEcoToNative((unsigned char *) &hp->ostrs);
  conv4FromEcoToNative((unsigned char *) &hp->sstrs);
  conv4FromEcoToNative((unsigned char *) &hp->entry);
  if (hp->magic != EXEC_MAGIC) {
    error("wrong magic number in input file '%s'", inName);
  }
}


char *readStrings(ExecHeader *hp, FILE *inFile, char *inName) {
  char *strs;

  strs = memAlloc(hp->sstrs);
  if (fseek(inFile, hp->ostrs, SEEK_SET) < 0) {
    error("cannot seek to strings in input file '%s'", inName);
  }
  if (fread(strs, 1, hp->sstrs, inFile) != hp->sstrs) {
    error("cannot read strings in input file '%s'", inName);
  }
  return strs;
}


void readSegments(int nsegs, SegmentRecord *segs,
                  unsigned int off, FILE *inFile, char *inName) {
  int i;

  if (fseek(inFile, off, SEEK_SET) < 0) {
    error("cannot seek to segment table in input file '%s'", inName);
  }
  for (i = 0; i < nsegs; i++) {
    if (fread(segs + i, sizeof(SegmentRecord), 1, inFile) != 1) {
      error("cannot read segment record in input file '%s'", inName);
    }
    conv4FromEcoToNative((unsigned char *) &segs[i].name);
    conv4FromEcoToNative((unsigned char *) &segs[i].offs);
    conv4FromEcoToNative((unsigned char *) &segs[i].addr);
    conv4FromEcoToNative((unsigned char *) &segs[i].size);
    conv4FromEcoToNative((unsigned char *) &segs[i].attr);
  }
}


void readAllIsegs(void) {
  Module *mp;
  FILE *inFile;
  ExecHeader hdr;

  mp = allModules;
  while (mp != NULL) {
    inFile = fopen(mp->name, "r");
    if (inFile == NULL) {
      error("cannot open input file '%s'", mp->name);
    }
    readHeader(&hdr, inFile, mp->name);
    mp->strs = readStrings(&hdr, inFile, mp->name);
    mp->nsegs = hdr.nsegs;
    mp->segtbl = memAlloc(hdr.nsegs * sizeof(SegmentRecord));
    readSegments(mp->nsegs, mp->segtbl, hdr.osegs, inFile, mp->name);
    fclose(inFile);
    mp = mp->next;
  }
}


void showAllIsegs(void) {
  Module *mp;
  int i;
  char attr[10];

  mp = allModules;
  while (mp != NULL) {
    printf("%s:\n", mp->name);
    for (i = 0; i < mp->nsegs; i++) {
      attr[0] = (mp->segtbl[i].attr & SEG_ATTR_A) ? 'A' : '-';
      attr[1] = (mp->segtbl[i].attr & SEG_ATTR_P) ? 'P' : '-';
      attr[2] = (mp->segtbl[i].attr & SEG_ATTR_W) ? 'W' : '-';
      attr[3] = (mp->segtbl[i].attr & SEG_ATTR_X) ? 'X' : '-';
      attr[4] = '\0';
      printf("iseg %s, addr = 0x%08X, size = 0x%08X, attr = [%s]\n",
             mp->strs + mp->segtbl[i].name,
             mp->segtbl[i].addr,
             mp->segtbl[i].size,
             attr);
    }
    mp = mp->next;
  }
}


/**************************************************************/


typedef struct iseg {
  char *name;
  unsigned int size;
  struct iseg *next;
} Iseg;


Iseg *isegTbl = NULL;


Iseg *lookupIsegTbl(char *name) {
  Iseg *p;

  p = isegTbl;
  while (p != NULL) {
    if (strcmp(p->name, name) == 0) {
      return p;
    }
    p = p->next;
  }
  return NULL;
}


Iseg *insertIsegTbl(char *name) {
  Iseg *p;

  p = lookupIsegTbl(name);
  if (p != NULL) {
    error("linker script tries to allocate segment '%s' more than once",
          name);
  }
  p = memAlloc(sizeof(Iseg));
  p->name = name;
  p->size = 0;
  p->next = isegTbl;
  isegTbl = p;
  return p;
}


/**************************************************************/


typedef struct oseg {
  char *name;
  unsigned int addr;
  unsigned int size;
  unsigned int attr;
  struct oseg *next;
} Oseg;


Oseg *allOsegs = NULL;
Oseg *lastOseg;


Oseg *newOseg(char *name, unsigned int attr) {
  Oseg *p;

  p = memAlloc(sizeof(Oseg));
  p->name = name;
  p->addr = 0;
  p->size = 0;
  p->attr = attr;
  p->next = NULL;
  if (allOsegs == NULL) {
    allOsegs = p;
  } else {
    lastOseg->next = p;
  }
  lastOseg = p;
  return p;
}


void showAllOsegs(void) {
  Oseg *p;
  char attr[10];

  p = allOsegs;
  while (p != NULL) {
    attr[0] = (p->attr & SEG_ATTR_A) ? 'A' : '-';
    attr[1] = (p->attr & SEG_ATTR_P) ? 'P' : '-';
    attr[2] = (p->attr & SEG_ATTR_W) ? 'W' : '-';
    attr[3] = (p->attr & SEG_ATTR_X) ? 'X' : '-';
    attr[4] = '\0';
    printf("oseg %s, addr = 0x%08X, size = 0x%08X, attr = [%s]\n",
           p->name, p->addr, p->size, attr);
    p = p->next;
  }
}


/**************************************************************/


unsigned int dot;


unsigned int evalExp(ScriptNode *exp);


unsigned int evalBinopExp(ScriptNode *exp) {
  unsigned int val1;
  unsigned int val2;

  val1 = evalExp(exp->u.binopExp.left);
  val2 = evalExp(exp->u.binopExp.right);
  switch (exp->u.binopExp.op) {
    case NODE_BINOP_OR:
      return val1 | val2;
    case NODE_BINOP_XOR:
      return val1 ^ val2;
    case NODE_BINOP_AND:
      return val1 & val2;
    case NODE_BINOP_LSH:
      return val1 << val2;
    case NODE_BINOP_RSH:
      return val1 >> val2;
    case NODE_BINOP_ADD:
      return val1 + val2;
    case NODE_BINOP_SUB:
      return val1 - val2;
    case NODE_BINOP_MUL:
      return val1 * val2;
    case NODE_BINOP_DIV:
      return val1 / val2;
    case NODE_BINOP_MOD:
      return val1 % val2;
    default:
      error("evalBinopExp() got invalid operator %d",
            exp->u.binopExp.op);
  }
  /* never reached */
  return 0;
}


unsigned int evalUnopExp(ScriptNode *exp) {
  unsigned int val;

  val = evalExp(exp->u.unopExp.exp);
  switch (exp->u.unopExp.op) {
    case NODE_UNOP_PLUS:
      return val;
    case NODE_UNOP_MINUS:
      return -val;
    case NODE_UNOP_COMPL:
      return ~val;
    default:
      error("evalUnopExp() got invalid operator %d",
            exp->u.unopExp.op);
  }
  /* never reached */
  return 0;
}


unsigned int evalIntExp(ScriptNode *exp) {
  return exp->u.intExp.val;
}


unsigned int evalVarExp(ScriptNode *exp) {
  if (strcmp(exp->u.varExp.name, ".") == 0) {
    /* variable is dot */
    return dot;
  } else {
    /* variable is not dot */
    error("symbol '%s' not allowed in expression",
          exp->u.varExp.name);
    /* never reached */
    return 0;
  }
}


unsigned int evalExp(ScriptNode *exp) {
  if (exp == NULL) {
    error("evalExp() got NULL pointer");
  }
  switch (exp->type) {
    case NODE_BINOPEXP:
      return evalBinopExp(exp);
    case NODE_UNOPEXP:
      return evalUnopExp(exp);
    case NODE_INTEXP:
      return evalIntExp(exp);
    case NODE_VAREXP:
      return evalVarExp(exp);
    default:
      error("evalExp() got invalid node type %d",
            exp->type);
  }
  /* never reached */
  return 0;
}


void doStm(ScriptNode *stm);


void doStmList(ScriptNode *stm) {
  while (stm != NULL) {
    doStm(stm->u.stmList.head);
    stm = stm->u.stmList.tail;
  }
}


void doEntryStm(ScriptNode *stm) {
  warning("doEntryStm() not implemented yet");
}


void doIsegStm(ScriptNode *stm) {
  Iseg *p;

  p = lookupIsegTbl(stm->u.isegStm.name);
  if (p == NULL) {
    /* this should never happen */
    error("'%s' vanished from iseg table", stm->u.isegStm.name);
  }
  dot += p->size;
}


void doOsegStm(ScriptNode *stm) {
  Oseg *p;
  unsigned int start;

  p = newOseg(stm->u.osegStm.name, stm->u.osegStm.attr);
  start = dot;
  doStm(stm->u.osegStm.stms);
  p->size = dot - start;
  p->addr = start;
}


void doAssignStm(ScriptNode *stm) {
  unsigned int val;

  if (strcmp(stm->u.assignStm.name, ".") == 0) {
    /* LHS is dot */
    val = evalExp(stm->u.assignStm.exp);
    dot = val;
  } else {
    /* LHS is not dot */
    warning("assignment to symbol '%s' not implemented yet",
            stm->u.assignStm.name);
  }
}


void doStm(ScriptNode *stm) {
  if (stm == NULL) {
    /* end of any list */
    return;
  }
  switch (stm->type) {
    case NODE_STMLIST:
      doStmList(stm);
      break;
    case NODE_ENTRYSTM:
      doEntryStm(stm);
      break;
    case NODE_ISEGSTM:
      doIsegStm(stm);
      break;
    case NODE_OSEGSTM:
      doOsegStm(stm);
      break;
    case NODE_ASSIGNSTM:
      doAssignStm(stm);
      break;
    default:
      error("doStm() got invalid node type %d",
            stm->type);
  }
}


/**************************************************************/


void buildIsegTbl(ScriptNode *script) {
  ScriptNode *node1;
  ScriptNode *node2;

  node1 = script;
  if (node1 == NULL) {
    warning("empty linker script");
    return;
  }
  if (node1->type != NODE_STMLIST) {
    /* this should never happen */
    error("malformed script tree");
  }
  while (node1 != NULL) {
    if (node1->u.stmList.head->type == NODE_OSEGSTM) {
      node2 = node1->u.stmList.head->u.osegStm.stms;
      while (node2 != NULL) {
        if (node2->u.stmList.head->type == NODE_ISEGSTM) {
          insertIsegTbl(node2->u.stmList.head->u.isegStm.name);
        }
        node2 = node2->u.stmList.tail;
      }
    }
    node1 = node1->u.stmList.tail;
  }
}


void setRelSegAddrs(void) {
  Module *mp;
  int i;
  char *segName;
  Iseg *segPtr;

  mp = allModules;
  while (mp != NULL) {
    for (i = 0; i < mp->nsegs; i++) {
      segName = mp->strs + mp->segtbl[i].name;
      segPtr = lookupIsegTbl(segName);
      if (segPtr == NULL) {
        /* input segment name does not match any ISEG name in script */
        warning("discarding segment '%s' from module '%s'",
                segName, mp->name);
      } else {
        /* set segment's relative addr, add segment's size to total */
        mp->segtbl[i].addr = segPtr->size;
        segPtr->size += mp->segtbl[i].size;
        segPtr->size = WORD_ALIGN(segPtr->size);
      }
    }
    mp = mp->next;
  }
}


void setAbsSegAddrs(ScriptNode *script) {
  dot = 0;
  doStm(script);
}


void allocateStorage(ScriptNode *script) {
 /* for each ISEG statement allocate an Iseg record */
  buildIsegTbl(script);
  /* set relative addrs of all input segments, sum up sizes */
  setRelSegAddrs();
  /* set absolute addrs by working through linker script */
  setAbsSegAddrs(script);
}


/**************************************************************/


void writeOutput(char *outName) {
  FILE *outFile;

  printf("%s:\n", outName);
  showAllOsegs();
  outFile = fopen(outName, "w");
  if (outFile == NULL) {
    error("cannot open output file '%s'", outName);
  }
  fclose(outFile);
}


/**************************************************************/


void writeMap(char *mapName) {
  FILE *mapFile;

  mapFile = fopen(mapName, "w");
  if (mapFile == NULL) {
    error("cannot open map file '%s'", mapName);
  }
  fclose(mapFile);
}


/**************************************************************/


void usage(char *myself) {
  fprintf(stderr, "Usage: %s\n", myself);
  fprintf(stderr, "         [-s scrfile]     set script file name\n");
  fprintf(stderr, "         [-o objfile]     set output file name\n");
  fprintf(stderr, "         [-m mapfile]     set map file name\n");
  fprintf(stderr, "         file             object file name\n");
  fprintf(stderr, "         [files...]       additional obj/lib files\n");
  exit(1);
}


int main(int argc, char *argv[]) {
  int i;
  char *argp;
  char *scrName;
  char *outName;
  char *mapName;
  ScriptNode *script;

  /* ----------- start discard ---------- */
  for (i = 0; i < argc; i++) {
    printf("arg %d: %s\n", i, argv[i]);
  }
  /* -----------  end discard  ---------- */
  scrName = DEFAULT_SCRIPT_NAME;
  outName = DEFAULT_OUT_NAME;
  mapName = NULL;
  for (i = 1; i < argc; i++) {
    argp = argv[i];
    if (*argp == '-') {
      /* option */
      argp++;
      switch (*argp) {
        case 's':
          if (i == argc - 1) {
            usage(argv[0]);
          }
          scrName = argv[++i];
          break;
        case 'o':
          if (i == argc - 1) {
            usage(argv[0]);
          }
          outName = argv[++i];
          break;
        case 'm':
          if (i == argc - 1) {
            usage(argv[0]);
          }
          mapName = argv[++i];
          break;
        default:
          usage(argv[0]);
      }
    } else {
      /* file */
      newModule(argp);
    }
  }
  if (allModules == NULL) {
    error("no input files");
  }
  script = readScript(scrName);
  showScript(script);
  readAllIsegs();
  showAllIsegs();
  allocateStorage(script);
  showAllIsegs();
  //------------------------
  writeOutput(outName);
  if (mapName != NULL) {
    writeMap(mapName);
  }
  return 0;
}
