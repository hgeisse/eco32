/*
 * ld.c -- ECO32 linking loader
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../include/a.out.h"
#include "../include/ar.h"
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


typedef struct file {
  char *path;
  struct file *next;
} File;


File *allFiles = NULL;
File *lastFile;


File *newFile(char *path) {
  File *fp;

  fp = memAlloc(sizeof(File));
  fp->path = path;
  fp->next = NULL;
  if (allFiles == NULL) {
    allFiles = fp;
  } else {
    lastFile->next = fp;
  }
  lastFile = fp;
  return fp;
}


/**************************************************************/


typedef struct module {
  char *name;
  char *strs;
  int nsegs;
  SegmentRecord *segtbl;
  int nsyms;
  struct sym **syms;
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
  mp->nsyms = 0;
  mp->syms = NULL;
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


typedef struct isegGrp {
  char *name;
  unsigned int addr;
  unsigned int size;
  struct isegGrp *next;
} IsegGrp;


IsegGrp *isegGrpTbl = NULL;


IsegGrp *lookupIsegGrp(char *name) {
  IsegGrp *p;

  p = isegGrpTbl;
  while (p != NULL) {
    if (strcmp(p->name, name) == 0) {
      return p;
    }
    p = p->next;
  }
  return NULL;
}


IsegGrp *addIsegGrp(char *name) {
  IsegGrp *p;

  p = lookupIsegGrp(name);
  if (p != NULL) {
    error("linker script allocates segment group '%s' more than once",
          name);
  }
  p = memAlloc(sizeof(IsegGrp));
  p->name = name;
  p->addr = 0;
  p->size = 0;
  p->next = isegGrpTbl;
  isegGrpTbl = p;
  return p;
}


/**************************************************************/

/*
 * global symbol table
 */


#define INITIAL_NUM_BUCKETS	100


typedef struct sym {
  /* symbol data */
  char *name;		/* the symbol's name */
  Module *mod;		/* symbol is defined there */
  int seg;		/* within this segment (-1 for absolute) */
  int val;		/* the symbol's value */
  unsigned int attr;	/* the symbol's attributes */
  /* internal data */
  unsigned int hash;	/* full hash over symbol's name */
  struct sym *next;	/* hash bucket chain */
} Sym;


static Sym **buckets = NULL;
static int numBuckets;
static int numEntries;


static unsigned int hash(char *s) {
  unsigned int h, g;

  h = 0;
  while (*s != '\0') {
    h = (h << 4) + *s++;
    g = h & 0xF0000000;
    if (g != 0) {
      h ^= g >> 24;
      h ^= g;
    }
  }
  return h;
}


static int isPrime(int i) {
  int t;

  if (i < 2) {
    return 0;
  }
  if (i == 2) {
    return 1;
  }
  if (i % 2 == 0) {
    return 0;
  }
  t = 3;
  while (t * t <= i) {
    if (i % t == 0) {
      return 0;
    }
    t += 2;
  }
  return 1;
}


static void initTable(void) {
  int i;

  numBuckets = INITIAL_NUM_BUCKETS;
  while (!isPrime(numBuckets)) {
    numBuckets++;
  }
  buckets = (Sym **) memAlloc(numBuckets * sizeof(Sym *));
  for (i = 0; i < numBuckets; i++) {
    buckets[i] = NULL;
  }
  numEntries = 0;
}


static void growTable(void) {
  int newNumBuckets;
  Sym **newBuckets;
  int i, n;
  Sym *p, *q;

  /* compute new hash size */
  newNumBuckets = 2 * numBuckets + 1;
  while (!isPrime(newNumBuckets)) {
    newNumBuckets += 2;
  }
  /* init new hash table */
  newBuckets = (Sym **) memAlloc(newNumBuckets * sizeof(Sym *));
  for (i = 0; i < newNumBuckets; i++) {
    newBuckets[i] = NULL;
  }
  /* rehash old entries */
  for (i = 0; i < numBuckets; i++) {
    p = buckets[i];
    while (p != NULL) {
      q = p;
      p = p->next;
      n = q->hash % newNumBuckets;
      q->next = newBuckets[n];
      newBuckets[n] = q;
    }
  }
  /* swap tables */
  memFree(buckets);
  buckets = newBuckets;
  numBuckets = newNumBuckets;
}


Sym *lookupSymbol(char *name) {
  unsigned int h;
  int n;
  Sym *p;

  /* initialize hash table if necessary */
  if (buckets == NULL) {
    initTable();
  }
  /* compute hash value and bucket number */
  h = hash(name);
  n = h % numBuckets;
  /* search in bucket list */
  p = buckets[n];
  while (p != NULL) {
    if (p->hash == h) {
      if (strcmp(p->name, name) == 0) {
        /* found: return symbol */
        return p;
      }
    }
    p = p->next;
  }
  /* not found: return NULL */
  return NULL;
}


Sym *enterSymbol(char *name) {
  unsigned int h;
  int n;
  Sym *p;

  /* initialize hash table if necessary */
  if (buckets == NULL) {
    initTable();
  }
  /* compute hash value and bucket number */
  h = hash(name);
  n = h % numBuckets;
  /* search in bucket list */
  p = buckets[n];
  while (p != NULL) {
    if (p->hash == h) {
      if (strcmp(p->name, name) == 0) {
        /* found: return symbol */
        return p;
      }
    }
    p = p->next;
  }
  /* not found: add new symbol to bucket list */
  /* grow hash table if necessary */
  if (numEntries == numBuckets) {
    growTable();
  }
  /* allocate new sym and link to bucket list */
  p = (Sym *) memAlloc(sizeof(Sym));
  p->name = name;
  p->mod = NULL;
  p->seg = -1;
  p->val = 0;
  p->attr = SYM_ATTR_U;
  p->hash = h;
  p->next = buckets[n];
  buckets[n] = p;
  numEntries++;
  return p;
}


void mapOverSymbols(void (*fp)(Sym *sym, void *arg), void *arg) {
  int i;
  Sym *sym;

  for (i = 0; i < numBuckets; i++) {
    sym = buckets[i];
    while (sym != NULL) {
      (*fp)(sym, arg);
      sym = sym->next;
    }
  }
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


void readObjHeader(ExecHeader *hp, unsigned int inOff,
                   FILE *inFile, char *inName) {
  if (fseek(inFile, inOff, SEEK_SET) < 0) {
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


char *readObjStrings(ExecHeader *hp, unsigned int inOff,
                     FILE *inFile, char *inName) {
  char *strs;

  strs = memAlloc(hp->sstrs);
  if (fseek(inFile, inOff + hp->ostrs, SEEK_SET) < 0) {
    error("cannot seek to strings in input file '%s'", inName);
  }
  if (fread(strs, 1, hp->sstrs, inFile) != hp->sstrs) {
    error("cannot read strings in input file '%s'", inName);
  }
  return strs;
}


void readObjSegments(int nsegs, SegmentRecord *segs,
                     unsigned int inOff, FILE *inFile, char *inName) {
  int i;

  if (fseek(inFile, inOff, SEEK_SET) < 0) {
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


void readObjSymbols(Module *mp, unsigned int inOff, FILE *inFile) {
  int i;
  SymbolRecord symbol;
  Sym *sym;

  if (fseek(inFile, inOff, SEEK_SET) < 0) {
    error("cannot seek to symbol table in input file '%s'", mp->name);
  }
  for (i = 0; i < mp->nsyms; i++) {
    if (fread(&symbol, sizeof(SymbolRecord), 1, inFile) != 1) {
      error("cannot read symbol record in input file '%s'", mp->name);
    }
    conv4FromEcoToNative((unsigned char *) &symbol.name);
    conv4FromEcoToNative((unsigned char *) &symbol.val);
    conv4FromEcoToNative((unsigned char *) &symbol.seg);
    conv4FromEcoToNative((unsigned char *) &symbol.attr);
    sym = enterSymbol(mp->strs + symbol.name);
    if ((symbol.attr & SYM_ATTR_U) == 0) {
      /* symbol gets defined here */
      if ((sym->attr & SYM_ATTR_U) == 0) {
        /* but is already defined in table */
        error("symbol '%s' in module '%s' defined more than once\n"
              "       (previous definition is in module '%s')",
              sym->name, mp->name, sym->mod->name);
      }
      /* copy symbol information to table */
      sym->mod = mp;
      sym->seg = symbol.seg;
      sym->val = symbol.val;
      sym->attr = symbol.attr;
    }
    mp->syms[i] = sym;
  }
}


void readObjModule(char *name, unsigned int inOff,
                   FILE *inFile, char *inName) {
  Module *mp;
  ExecHeader hdr;

  printf("read module '%s' from file '%s' @ offset 0x%08X\n",
         name, inName, inOff);
  mp = newModule(name);
  readObjHeader(&hdr, inOff, inFile, mp->name);
  mp->strs = readObjStrings(&hdr, inOff, inFile, mp->name);
  mp->nsegs = hdr.nsegs;
  mp->segtbl = memAlloc(hdr.nsegs * sizeof(SegmentRecord));
  readObjSegments(mp->nsegs, mp->segtbl,
                  inOff + hdr.osegs, inFile, mp->name);
  mp->nsyms = hdr.nsyms;
  mp->syms = memAlloc(hdr.nsyms * sizeof(Sym *));
  readObjSymbols(mp, inOff + hdr.osyms, inFile);
}


/**************************************************************/


void readArchHeader(ArchHeader *hp, FILE *inFile, char *inName) {
  if (fseek(inFile, 0, SEEK_SET) < 0) {
    error("cannot seek to header in input file '%s'", inName);
  }
  if (fread(hp, sizeof(ArchHeader), 1, inFile) != 1) {
    error("cannot read header in input file '%s'", inName);
  }
  conv4FromEcoToNative((unsigned char *) &hp->magic);
  conv4FromEcoToNative((unsigned char *) &hp->omods);
  conv4FromEcoToNative((unsigned char *) &hp->nmods);
  conv4FromEcoToNative((unsigned char *) &hp->odata);
  conv4FromEcoToNative((unsigned char *) &hp->sdata);
  conv4FromEcoToNative((unsigned char *) &hp->ostrs);
  conv4FromEcoToNative((unsigned char *) &hp->sstrs);
  if (hp->magic != ARCH_MAGIC) {
    error("wrong magic number in input file '%s'", inName);
  }
}


char *readArchStrings(ArchHeader *hp, FILE *inFile, char *inName) {
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


void readArchModules(int nmods, ModuleRecord *mods,
                     unsigned int inOff, FILE *inFile, char *inName) {
  int i;

  if (fseek(inFile, inOff, SEEK_SET) < 0) {
    error("cannot seek to module table in input file '%s'", inName);
  }
  for (i = 0; i < nmods; i++) {
    if (fread(mods + i, sizeof(ModuleRecord), 1, inFile) != 1) {
      error("cannot read module record in input file '%s'", inName);
    }
    conv4FromEcoToNative((unsigned char *) &mods[i].name);
    conv4FromEcoToNative((unsigned char *) &mods[i].offs);
    conv4FromEcoToNative((unsigned char *) &mods[i].size);
    conv4FromEcoToNative((unsigned char *) &mods[i].fsym);
    conv4FromEcoToNative((unsigned char *) &mods[i].nsym);
  }
}


int needArchModule(char *fsym, unsigned int nsym) {
  int i;
  Sym *sym;

  for (i = 0; i < nsym; i++) {
    sym = lookupSymbol(fsym);
    if (sym != NULL && (sym->attr & SYM_ATTR_U) != 0) {
      /*
       * This symbol is undefined in the global symbol table,
       * so this module needs to be extracted.
       */
      return 1;
    }
    /* skip symbol name */
    while (*fsym++ != '\0') ;
  }
  /*
   * None of the modules's symbols were found undefined in
   * the global symbol table, so don't extract the module.
   */
  return 0;
}


void readArchive(FILE *inFile, char *inName) {
  ArchHeader hdr;
  char *strs;
  ModuleRecord *mods;
  int anyModuleExtracted;
  int i;
  char *moduleName;

  readArchHeader(&hdr, inFile, inName);
  strs = readArchStrings(&hdr, inFile, inName);
  mods = memAlloc(hdr.nmods * sizeof(ModuleRecord));
  readArchModules(hdr.nmods, mods, hdr.omods, inFile, inName);
  do {
    anyModuleExtracted = 0;
    for (i = 0; i < hdr.nmods; i++) {
      if (needArchModule(strs + mods[i].fsym, mods[i].nsym)) {
        /* store module name permanently and read module */
        moduleName = memAlloc(strlen(strs + mods[i].name) + 1);
        strcpy(moduleName, strs + mods[i].name);
        readObjModule(moduleName, hdr.odata + mods[i].offs,
                      inFile, inName);
        /* there is no reason to scan this module ever again */
        mods[i].nsym = 0;
        /* remember that we extracted a module: must repeat loop */
        anyModuleExtracted = 1;
      }
    }
  } while (anyModuleExtracted);
  memFree(mods);
  memFree(strs);
}


/**************************************************************/


void readFiles(void) {
  File *fp;
  FILE *inFile;
  unsigned int magic;
  char *baseName;

  fp = allFiles;
  while (fp != NULL) {
    inFile = fopen(fp->path, "r");
    if (inFile == NULL) {
      error("cannot open input file '%s'", fp->path);
    }
    if (fread(&magic, sizeof(unsigned int), 1, inFile) != 1) {
      error("cannot read magic number in input file '%s'", fp->path);
    }
    conv4FromEcoToNative((unsigned char *) &magic);
    if (magic == EXEC_MAGIC) {
      baseName = fp->path + strlen(fp->path);
      while (baseName != fp->path && *baseName != '/') {
        baseName--;
      }
      if (*baseName == '/') {
        baseName++;
      }
      readObjModule(baseName, 0, inFile, fp->path);
    } else
    if (magic == ARCH_MAGIC) {
      readArchive(inFile, fp->path);
    } else {
      error("input file '%s' is neither an object file nor a library",
            fp->path);
    }
    fclose(inFile);
    fp = fp->next;
  }
}


void showModules(void) {
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


unsigned int dot;
Sym *entry;


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
  entry = enterSymbol(stm->u.entryStm.name);
}


void doIsegStm(ScriptNode *stm) {
  IsegGrp *p;

  p = lookupIsegGrp(stm->u.isegStm.name);
  if (p == NULL) {
    /* this should never happen */
    error("iseg group '%s' vanished from iseg group table",
          stm->u.isegStm.name);
  }
  p->addr = dot;
  dot += p->size;
}


void doOsegStm(ScriptNode *stm) {
  Oseg *p;
  unsigned int start;

  p = newOseg(stm->u.osegStm.name, stm->u.osegStm.attr);
  start = dot;
  doStm(stm->u.osegStm.stms);
  p->addr = start;
  p->size = dot - start;
}


/*
 * This is a dummy module record in order to have a home
 * for variables which get defined in the linker script.
 */
static Module scriptModule = {
  "linker script",
  NULL, 0, NULL, 0, NULL, NULL
};


void doAssignStm(ScriptNode *stm) {
  unsigned int val;
  Sym *sym;

  val = evalExp(stm->u.assignStm.exp);
  if (strcmp(stm->u.assignStm.name, ".") == 0) {
    /* LHS is dot */
    dot = val;
  } else {
    /* LHS is not dot */
    sym = enterSymbol(stm->u.assignStm.name);
    if ((sym->attr & SYM_ATTR_U) == 0) {
      /* symbol is already defined in table */
      error("symbol '%s' in linker script defined more than once\n"
            "       (previous definition is in module '%s')",
            sym->name, sym->mod->name);
    }
    /* copy symbol information to table */
    sym->mod = &scriptModule;
    sym->seg = -1;
    sym->val = val;
    sym->attr = 0;
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


void buildIsegGrpTbl(ScriptNode *script) {
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
          addIsegGrp(node2->u.stmList.head->u.isegStm.name);
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
  IsegGrp *grpPtr;

  mp = allModules;
  while (mp != NULL) {
    for (i = 0; i < mp->nsegs; i++) {
      segName = mp->strs + mp->segtbl[i].name;
      grpPtr = lookupIsegGrp(segName);
      if (grpPtr == NULL) {
        /* input segment name does not match any ISEG name in script */
        warning("discarding segment '%s' from module '%s'",
                segName, mp->name);
      } else {
        /* set segment's relative address in group */
        mp->segtbl[i].addr = grpPtr->size;
        /* add segment's size to group total */
        grpPtr->size += mp->segtbl[i].size;
        grpPtr->size = WORD_ALIGN(grpPtr->size);
      }
    }
    mp = mp->next;
  }
}


void setAbsSegAddrs(void) {
  Module *mp;
  int i;
  char *segName;
  IsegGrp *grpPtr;

  mp = allModules;
  while (mp != NULL) {
    for (i = 0; i < mp->nsegs; i++) {
      segName = mp->strs + mp->segtbl[i].name;
      grpPtr = lookupIsegGrp(segName);
      if (grpPtr != NULL) {
        /* add group start to segment's relative address */
        mp->segtbl[i].addr += grpPtr->addr;
      }
    }
    mp = mp->next;
  }
}


void allocateStorage(ScriptNode *script) {
  /* for each ISEG statement in the linker script
     allocate a group record */
  buildIsegGrpTbl(script);
  /* set group relative addresses of all input segments,
     compute total sizes of all groups */
  setRelSegAddrs();
  /* set group start and output segment addresses by
     working through the linker script */
  dot = 0;
  entry = NULL;
  doStm(script);
  /* set absolute addresses of all input segments */
  setAbsSegAddrs();
}


/**************************************************************/


static void countUndefSymbol(Sym *sym, void *arg) {
  if (sym->attr & SYM_ATTR_U) {
    (*(int *)arg)++;
  }
}


int countUndefSymbols(void) {
  int numUndefSymbols;

  numUndefSymbols = 0;
  mapOverSymbols(countUndefSymbol, (void *) &numUndefSymbols);
  return numUndefSymbols;
}


static void showUndefSymbol(Sym *sym, void *arg) {
  if (sym->attr & SYM_ATTR_U) {
    printf("    %s\n", sym->name);
  }
}


void showUndefSymbols(void) {
  mapOverSymbols(showUndefSymbol, NULL);
}


static void showSymbol(Sym *sym, void *arg) {
  Module *mp;

  mp = sym->mod;
  printf("    %s: mod = %s, seg = %s, val = 0x%08X\n",
         sym->name,
         mp->name,
         sym->seg == -1 ? "*ABS*" : mp->strs + mp->segtbl[sym->seg].name,
         sym->val);
}


void showAllSymbols(void) {
  printf("Global Symbol Table\n");
  mapOverSymbols(showSymbol, NULL);
}


void resolveSymbol(Sym *sym, void *arg) {
  Module *modPtr;
  SegmentRecord *segPtr;

  if (sym->seg == -1) {
    /* absolute symbol: keep value */
    return;
  }
  /* add segment address to symbol value */
  modPtr = sym->mod;
  segPtr = modPtr->segtbl + sym->seg;
  sym->val += segPtr->addr;
}


void resolveSymbols(void) {
  int n;

  n = countUndefSymbols();
  if (n > 0) {
    printf("The following symbols are undefined:\n");
    showUndefSymbols();
    error("%d undefined symbol(s)", n);
  }
  mapOverSymbols(resolveSymbol, NULL);
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
  fprintf(stderr, "         [file]           additional obj/lib file\n");
  fprintf(stderr, "         [-Ldir]          additional lib directory\n");
  fprintf(stderr, "         [-lfile]         additional obj/lib file\n");
  fprintf(stderr, "                          file name is 'lib<file>.a'\n");
  fprintf(stderr, "                          searched in any dir from -L\n");
  exit(1);
}


int main(int argc, char *argv[]) {
  int i;
  char *argp;
  char *scrName;
  char *outName;
  char *mapName;
  ScriptNode *script;
  char *libName;

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
        case 'L':
          argp++;
          if (*argp == '\0') {
            usage(argv[0]);
          }
          break;
        case 'l':
          argp++;
          if (*argp == '\0') {
            usage(argv[0]);
          }
          libName = memAlloc(3 + strlen(argp) + 3);
          strcpy(libName, "lib");
          strcat(libName, argp);
          strcat(libName, ".a");
          newFile(libName);
          break;
        default:
          usage(argv[0]);
      }
    } else {
      /* file */
      newFile(argp);
    }
  }
  if (allFiles == NULL) {
    error("no input files");
  }
  script = readScript(scrName);
  showScript(script);
  readFiles();
  allocateStorage(script);
  showModules();
  resolveSymbols();
  showAllSymbols();
  //----------------------------
  writeOutput(outName);
  if (mapName != NULL) {
    writeMap(mapName);
  }
  return 0;
}
