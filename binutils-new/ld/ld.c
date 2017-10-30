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


typedef struct file {
  char *path;
  struct file *next;
} File;


File *allFiles = NULL;
File *lastFile;


File *newFile(char *path) {
  File *file;

  file = memAlloc(sizeof(File));
  file->path = path;
  file->next = NULL;
  if (allFiles == NULL) {
    allFiles = file;
  } else {
    lastFile->next = file;
  }
  lastFile = file;
  return file;
}


/**************************************************************/


typedef struct module {
  char *name;
  char *strs;
  int nsegs;
  struct seg *segs;
  int nsyms;
  struct sym **syms;
  struct module *next;
} Module;


Module *allModules = NULL;
Module *lastModule;


Module *newModule(char *name) {
  Module *mod;

  mod = memAlloc(sizeof(Module));
  mod->name = name;
  mod->strs = NULL;
  mod->nsegs = 0;
  mod->segs = NULL;
  mod->nsyms = 0;
  mod->syms = NULL;
  mod->next = NULL;
  if (allModules == NULL) {
    allModules = mod;
  } else {
    lastModule->next = mod;
  }
  lastModule = mod;
  return mod;
}


/**************************************************************/


typedef struct seg {
  char *name;
  unsigned int addr;
  unsigned int size;
  unsigned int attr;
} Seg;


/**************************************************************/


typedef struct isegGrp {
  char *name;
  unsigned int addr;
  unsigned int size;
  struct isegGrp *next;
} IsegGrp;


IsegGrp *isegGrpTbl = NULL;


IsegGrp *lookupIsegGrp(char *name) {
  IsegGrp *grp;

  grp = isegGrpTbl;
  while (grp != NULL) {
    if (strcmp(grp->name, name) == 0) {
      return grp;
    }
    grp = grp->next;
  }
  return NULL;
}


IsegGrp *addIsegGrp(char *name) {
  IsegGrp *grp;

  grp = lookupIsegGrp(name);
  if (grp != NULL) {
    error("linker script allocates segment group '%s' more than once",
          name);
  }
  grp = memAlloc(sizeof(IsegGrp));
  grp->name = name;
  grp->addr = 0;
  grp->size = 0;
  grp->next = isegGrpTbl;
  isegGrpTbl = grp;
  return grp;
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
  Oseg *oseg;

  oseg = memAlloc(sizeof(Oseg));
  oseg->name = name;
  oseg->addr = 0;
  oseg->size = 0;
  oseg->attr = attr;
  oseg->next = NULL;
  if (allOsegs == NULL) {
    allOsegs = oseg;
  } else {
    lastOseg->next = oseg;
  }
  lastOseg = oseg;
  return oseg;
}


void showAllOsegs(void) {
  Oseg *oseg;
  char attr[10];

  oseg = allOsegs;
  while (oseg != NULL) {
    attr[0] = (oseg->attr & SEG_ATTR_A) ? 'A' : '-';
    attr[1] = (oseg->attr & SEG_ATTR_P) ? 'P' : '-';
    attr[2] = (oseg->attr & SEG_ATTR_W) ? 'W' : '-';
    attr[3] = (oseg->attr & SEG_ATTR_X) ? 'X' : '-';
    attr[4] = '\0';
    printf("oseg %s, addr = 0x%08X, size = 0x%08X, attr = [%s]\n",
           oseg->name, oseg->addr, oseg->size, attr);
    oseg = oseg->next;
  }
}


/**************************************************************/


void readObjHeader(ExecHeader *hdr, unsigned int inOff,
                   FILE *inFile, char *inPath) {
  if (fseek(inFile, inOff, SEEK_SET) < 0) {
    error("cannot seek to header in input file '%s'", inPath);
  }
  if (fread(hdr, sizeof(ExecHeader), 1, inFile) != 1) {
    error("cannot read header in input file '%s'", inPath);
  }
  conv4FromEcoToNative((unsigned char *) &hdr->magic);
  conv4FromEcoToNative((unsigned char *) &hdr->osegs);
  conv4FromEcoToNative((unsigned char *) &hdr->nsegs);
  conv4FromEcoToNative((unsigned char *) &hdr->osyms);
  conv4FromEcoToNative((unsigned char *) &hdr->nsyms);
  conv4FromEcoToNative((unsigned char *) &hdr->orels);
  conv4FromEcoToNative((unsigned char *) &hdr->nrels);
  conv4FromEcoToNative((unsigned char *) &hdr->odata);
  conv4FromEcoToNative((unsigned char *) &hdr->sdata);
  conv4FromEcoToNative((unsigned char *) &hdr->ostrs);
  conv4FromEcoToNative((unsigned char *) &hdr->sstrs);
  conv4FromEcoToNative((unsigned char *) &hdr->entry);
  if (hdr->magic != EXEC_MAGIC) {
    error("wrong magic number in input file '%s'", inPath);
  }
}


char *readObjStrings(ExecHeader *hdr, unsigned int inOff,
                     FILE *inFile, char *inPath) {
  char *strs;

  strs = memAlloc(hdr->sstrs);
  if (fseek(inFile, inOff + hdr->ostrs, SEEK_SET) < 0) {
    error("cannot seek to strings in input file '%s'", inPath);
  }
  if (fread(strs, 1, hdr->sstrs, inFile) != hdr->sstrs) {
    error("cannot read strings in input file '%s'", inPath);
  }
  return strs;
}


void readObjSegments(Module *mod, unsigned int inOff,
                     FILE *inFile, char *inPath) {
  int i;
  SegmentRecord segment;

  if (fseek(inFile, inOff, SEEK_SET) < 0) {
    error("cannot seek to segment table in input file '%s'", inPath);
  }
  for (i = 0; i < mod->nsegs; i++) {
    if (fread(&segment, sizeof(SegmentRecord), 1, inFile) != 1) {
      error("cannot read segment record in input file '%s'", inPath);
    }
    conv4FromEcoToNative((unsigned char *) &segment.name);
    conv4FromEcoToNative((unsigned char *) &segment.offs);
    conv4FromEcoToNative((unsigned char *) &segment.addr);
    conv4FromEcoToNative((unsigned char *) &segment.size);
    conv4FromEcoToNative((unsigned char *) &segment.attr);
    mod->segs[i].name = mod->strs + segment.name;
    mod->segs[i].addr = segment.addr;
    mod->segs[i].size = segment.size;
    mod->segs[i].attr = segment.attr;
  }
}


void readObjSymbols(Module *mod, unsigned int inOff,
                    FILE *inFile, char *inPath) {
  int i;
  SymbolRecord symbol;
  Sym *sym;

  if (fseek(inFile, inOff, SEEK_SET) < 0) {
    error("cannot seek to symbol table in input file '%s'", inPath);
  }
  for (i = 0; i < mod->nsyms; i++) {
    if (fread(&symbol, sizeof(SymbolRecord), 1, inFile) != 1) {
      error("cannot read symbol record in input file '%s'", inPath);
    }
    conv4FromEcoToNative((unsigned char *) &symbol.name);
    conv4FromEcoToNative((unsigned char *) &symbol.val);
    conv4FromEcoToNative((unsigned char *) &symbol.seg);
    conv4FromEcoToNative((unsigned char *) &symbol.attr);
    sym = enterSymbol(mod->strs + symbol.name);
    if ((symbol.attr & SYM_ATTR_U) == 0) {
      /* symbol gets defined here */
      if ((sym->attr & SYM_ATTR_U) == 0) {
        /* but is already defined in table */
        error("symbol '%s' in module '%s' defined more than once\n"
              "       (previous definition is in module '%s')",
              sym->name, mod->name, sym->mod->name);
      }
      /* copy symbol information to table */
      sym->mod = mod;
      sym->seg = symbol.seg;
      sym->val = symbol.val;
      sym->attr = symbol.attr;
    }
    mod->syms[i] = sym;
  }
}


void readObjModule(char *name, unsigned int inOff,
                   FILE *inFile, char *inPath) {
  Module *mod;
  ExecHeader hdr;

  printf("read module '%s' from file '%s' @ offset 0x%08X\n",
         name, inPath, inOff);
  mod = newModule(name);
  readObjHeader(&hdr, inOff, inFile, inPath);
  mod->strs = readObjStrings(&hdr, inOff, inFile, inPath);
  mod->nsegs = hdr.nsegs;
  mod->segs = memAlloc(hdr.nsegs * sizeof(Seg));
  readObjSegments(mod, inOff + hdr.osegs, inFile, inPath);
  mod->nsyms = hdr.nsyms;
  mod->syms = memAlloc(hdr.nsyms * sizeof(Sym *));
  readObjSymbols(mod, inOff + hdr.osyms, inFile, inPath);
}


/**************************************************************/


void readArchHeader(ArchHeader *hdr, FILE *inFile, char *inPath) {
  if (fseek(inFile, 0, SEEK_SET) < 0) {
    error("cannot seek to header in input file '%s'", inPath);
  }
  if (fread(hdr, sizeof(ArchHeader), 1, inFile) != 1) {
    error("cannot read header in input file '%s'", inPath);
  }
  conv4FromEcoToNative((unsigned char *) &hdr->magic);
  conv4FromEcoToNative((unsigned char *) &hdr->omods);
  conv4FromEcoToNative((unsigned char *) &hdr->nmods);
  conv4FromEcoToNative((unsigned char *) &hdr->odata);
  conv4FromEcoToNative((unsigned char *) &hdr->sdata);
  conv4FromEcoToNative((unsigned char *) &hdr->ostrs);
  conv4FromEcoToNative((unsigned char *) &hdr->sstrs);
  if (hdr->magic != ARCH_MAGIC) {
    error("wrong magic number in input file '%s'", inPath);
  }
}


char *readArchStrings(ArchHeader *hdr, FILE *inFile, char *inPath) {
  char *strs;

  strs = memAlloc(hdr->sstrs);
  if (fseek(inFile, hdr->ostrs, SEEK_SET) < 0) {
    error("cannot seek to strings in input file '%s'", inPath);
  }
  if (fread(strs, 1, hdr->sstrs, inFile) != hdr->sstrs) {
    error("cannot read strings in input file '%s'", inPath);
  }
  return strs;
}


void readArchModules(int nmods, ModuleRecord *mods,
                     unsigned int inOff, FILE *inFile, char *inPath) {
  int i;

  if (fseek(inFile, inOff, SEEK_SET) < 0) {
    error("cannot seek to module table in input file '%s'", inPath);
  }
  for (i = 0; i < nmods; i++) {
    if (fread(mods + i, sizeof(ModuleRecord), 1, inFile) != 1) {
      error("cannot read module record in input file '%s'", inPath);
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


void readArchive(FILE *inFile, char *inPath) {
  ArchHeader hdr;
  char *strs;
  ModuleRecord *mods;
  int anyModuleExtracted;
  int i;
  char *moduleName;

  readArchHeader(&hdr, inFile, inPath);
  strs = readArchStrings(&hdr, inFile, inPath);
  mods = memAlloc(hdr.nmods * sizeof(ModuleRecord));
  readArchModules(hdr.nmods, mods, hdr.omods, inFile, inPath);
  do {
    anyModuleExtracted = 0;
    for (i = 0; i < hdr.nmods; i++) {
      if (needArchModule(strs + mods[i].fsym, mods[i].nsym)) {
        /* store module name permanently and read module */
        moduleName = memAlloc(strlen(strs + mods[i].name) + 1);
        strcpy(moduleName, strs + mods[i].name);
        readObjModule(moduleName, hdr.odata + mods[i].offs,
                      inFile, inPath);
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
  File *file;
  FILE *inFile;
  unsigned int magic;
  char *baseName;

  file = allFiles;
  while (file != NULL) {
    inFile = fopen(file->path, "r");
    if (inFile == NULL) {
      error("cannot open input file '%s'", file->path);
    }
    if (fread(&magic, sizeof(unsigned int), 1, inFile) != 1) {
      error("cannot read magic number in input file '%s'", file->path);
    }
    conv4FromEcoToNative((unsigned char *) &magic);
    if (magic == EXEC_MAGIC) {
      baseName = file->path + strlen(file->path);
      while (baseName != file->path && *baseName != '/') {
        baseName--;
      }
      if (*baseName == '/') {
        baseName++;
      }
      readObjModule(baseName, 0, inFile, file->path);
    } else
    if (magic == ARCH_MAGIC) {
      readArchive(inFile, file->path);
    } else {
      error("input file '%s' is neither an object file nor a library",
            file->path);
    }
    fclose(inFile);
    file = file->next;
  }
}


void showModules(void) {
  Module *mod;
  int i;
  char attr[10];

  mod = allModules;
  while (mod != NULL) {
    printf("%s:\n", mod->name);
    for (i = 0; i < mod->nsegs; i++) {
      attr[0] = (mod->segs[i].attr & SEG_ATTR_A) ? 'A' : '-';
      attr[1] = (mod->segs[i].attr & SEG_ATTR_P) ? 'P' : '-';
      attr[2] = (mod->segs[i].attr & SEG_ATTR_W) ? 'W' : '-';
      attr[3] = (mod->segs[i].attr & SEG_ATTR_X) ? 'X' : '-';
      attr[4] = '\0';
      printf("iseg %s, addr = 0x%08X, size = 0x%08X, attr = [%s]\n",
             mod->segs[i].name,
             mod->segs[i].addr,
             mod->segs[i].size,
             attr);
    }
    mod = mod->next;
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
  IsegGrp *grp;

  grp = lookupIsegGrp(stm->u.isegStm.name);
  if (grp == NULL) {
    /* this should never happen */
    error("iseg group '%s' vanished from iseg group table",
          stm->u.isegStm.name);
  }
  grp->addr = dot;
  dot += grp->size;
}


void doOsegStm(ScriptNode *stm) {
  Oseg *oseg;
  unsigned int start;

  oseg = newOseg(stm->u.osegStm.name, stm->u.osegStm.attr);
  start = dot;
  doStm(stm->u.osegStm.stms);
  oseg->addr = start;
  oseg->size = dot - start;
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
  Module *mod;
  int i;
  char *segName;
  IsegGrp *grp;

  mod = allModules;
  while (mod != NULL) {
    for (i = 0; i < mod->nsegs; i++) {
      segName = mod->segs[i].name;
      grp = lookupIsegGrp(segName);
      if (grp == NULL) {
        /* input segment name does not match any ISEG name in script */
        warning("discarding segment '%s' from module '%s'",
                segName, mod->name);
      } else {
        /* set segment's relative address in group */
        mod->segs[i].addr = grp->size;
        /* add segment's size to group total */
        grp->size += mod->segs[i].size;
        grp->size = WORD_ALIGN(grp->size);
      }
    }
    mod = mod->next;
  }
}


void setAbsSegAddrs(void) {
  Module *mod;
  int i;
  char *segName;
  IsegGrp *grp;

  mod = allModules;
  while (mod != NULL) {
    for (i = 0; i < mod->nsegs; i++) {
      segName = mod->segs[i].name;
      grp = lookupIsegGrp(segName);
      if (grp != NULL) {
        /* add group start to segment's relative address */
        mod->segs[i].addr += grp->addr;
      }
    }
    mod = mod->next;
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
  Module *mod;

  mod = sym->mod;
  printf("    %s: mod = %s, seg = %s, val = 0x%08X\n",
         sym->name,
         mod->name,
         sym->seg == -1 ? "*ABS*" : mod->segs[sym->seg].name,
         sym->val);
}


void showAllSymbols(void) {
  printf("Global Symbol Table\n");
  mapOverSymbols(showSymbol, NULL);
}


void resolveSymbol(Sym *sym, void *arg) {
  Module *modPtr;
  Seg *segPtr;

  if (sym->seg == -1) {
    /* absolute symbol: keep value */
    return;
  }
  /* add segment address to symbol value */
  modPtr = sym->mod;
  segPtr = modPtr->segs + sym->seg;
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


void relocateModules(void) {
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
  relocateModules();
  //----------------------------
  writeOutput(outName);
  if (mapName != NULL) {
    writeMap(mapName);
  }
  return 0;
}
