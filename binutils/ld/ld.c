/*
 * ld.c -- ECO32 link editor
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

/*
 * data structure definitions
 */


typedef struct oseg {
  char *name;
  unsigned int addr;
  unsigned int size;
  unsigned int attr;
  unsigned int nameOffs;
  unsigned int dataOffs;
  struct igrp *firstIgrp;
  struct igrp *lastIgrp;
  struct oseg *next;
} Oseg;


typedef struct file {
  char *path;
  struct file *next;
} File;


typedef struct module {
  char *name;			/* module name */
  char *strs;			/* string space */
  unsigned char *data;		/* data space */
  int nsegs;			/* number of segments */
  SegmentRecord *segs;		/* array of segments */
  int nsyms;			/* number of symbols */
  struct sym **syms;		/* array of pointers to symbols */
  int nrels;			/* number of relocations */
  RelocRecord *rels;		/* array of relocations */
  struct module *next;		/* next module, order is important */
} Module;


typedef struct iseg {
  struct module *mod;
  int seg;
  struct iseg *next;
} Iseg;


typedef struct igrp {
  char *name;
  unsigned int addr;
  unsigned int size;
  struct iseg *firstIseg;	/* group is compoesed of these Isegs */
  struct iseg *lastIseg;	/* order is important */
  struct igrp *next;		/* next Igrp */
} Igrp;


typedef struct sym {
  /* symbol data */
  char *name;			/* the symbol's name */
  Module *mod;			/* symbol is defined there */
  int seg;			/* within this segment (-1 for absolute) */
  int val;			/* the symbol's value */
  unsigned int attr;		/* the symbol's attributes */
  /* internal data */
  unsigned int hash;		/* full hash over symbol's name */
  struct sym *next;		/* hash bucket chain */
} Sym;


/**************************************************************/


Oseg *firstOseg = NULL;
Oseg *lastOseg;


Oseg *newOseg(char *name) {
  Oseg *oseg;

  oseg = memAlloc(sizeof(Oseg));
  oseg->name = name;
  oseg->addr = 0;
  oseg->size = 0;
  oseg->attr = 0;
  oseg->nameOffs = 0;
  oseg->dataOffs = 0;
  oseg->firstIgrp = NULL;
  oseg->lastIgrp = NULL;
  oseg->next = NULL;
  if (firstOseg == NULL) {
    firstOseg = oseg;
  } else {
    lastOseg->next = oseg;
  }
  lastOseg = oseg;
  return oseg;
}


Oseg *findOseg(char *name) {
  Oseg *oseg;

  oseg = firstOseg;
  while (oseg != NULL) {
    if (strcmp(oseg->name, name) == 0) {
      /* oseg found */
      return oseg;
    }
    oseg = oseg->next;
  }
  /* oseg not found */
  return NULL;
}


void showAllOsegs(char *outName) {
  Oseg *oseg;
  char attr[10];
  Igrp *igrp;

  fprintf(stderr, "segments of output file '%s':\n", outName);
  oseg = firstOseg;
  while (oseg != NULL) {
    attr[0] = (oseg->attr & SEG_ATTR_A) ? 'A' : '-';
    attr[1] = (oseg->attr & SEG_ATTR_P) ? 'P' : '-';
    attr[2] = (oseg->attr & SEG_ATTR_W) ? 'W' : '-';
    attr[3] = (oseg->attr & SEG_ATTR_X) ? 'X' : '-';
    attr[4] = '\0';
    fprintf(stderr,
            "    %s : addr = 0x%08X, size = 0x%08X, attr = [%s]\n",
            oseg->name, oseg->addr, oseg->size, attr);
    fprintf(stderr, "        iseg groups:");
    igrp = oseg->firstIgrp;
    while (igrp != NULL) {
      fprintf(stderr, " %s", igrp->name);
      igrp = igrp->next;
    }
    fprintf(stderr, "\n");
    oseg = oseg->next;
  }
}


/**************************************************************/


File *firstFile = NULL;
File *lastFile;


File *newFile(char *path) {
  File *file;

  file = memAlloc(sizeof(File));
  file->path = path;
  file->next = NULL;
  if (firstFile == NULL) {
    firstFile = file;
  } else {
    lastFile->next = file;
  }
  lastFile = file;
  return file;
}


/**************************************************************/


Module *firstModule = NULL;
Module *lastModule;


Module *newModule(char *name) {
  Module *mod;

  mod = memAlloc(sizeof(Module));
  mod->name = name;
  mod->strs = NULL;
  mod->data = NULL;
  mod->nsegs = 0;
  mod->segs = NULL;
  mod->nsyms = 0;
  mod->syms = NULL;
  mod->nrels = 0;
  mod->rels = NULL;
  mod->next = NULL;
  if (firstModule == NULL) {
    firstModule = mod;
  } else {
    lastModule->next = mod;
  }
  lastModule = mod;
  return mod;
}


/**************************************************************/


Igrp *lookupIgrp(char *name) {
  Oseg *oseg;
  Igrp *igrp;

  oseg = firstOseg;
  while (oseg != NULL) {
    igrp = oseg->firstIgrp;
    while (igrp != NULL) {
      if (strcmp(igrp->name, name) == 0) {
        /* group found */
        return igrp;
      }
      igrp = igrp->next;
    }
    oseg = oseg->next;
  }
  /* group not found */
  return NULL;
}


Igrp *addIgrpToOseg(char *name, Oseg *oseg) {
  Igrp *igrp;

  igrp = lookupIgrp(name);
  if (igrp != NULL) {
    error("linker script allocates segment group '%s' more than once",
          name);
  }
  igrp = memAlloc(sizeof(Igrp));
  igrp->name = name;
  igrp->addr = 0;
  igrp->size = 0;
  igrp->firstIseg = NULL;
  igrp->lastIseg = NULL;
  igrp->next = NULL;
  if (oseg->firstIgrp == NULL) {
    oseg->firstIgrp = igrp;
  } else {
    oseg->lastIgrp->next = igrp;
  }
  oseg->lastIgrp = igrp;
  return igrp;
}


Iseg *addIsegToIgrp(Module *mod, int seg, Igrp *igrp) {
  Iseg *iseg;

  iseg = memAlloc(sizeof(Iseg));
  iseg->mod = mod;
  iseg->seg = seg;
  iseg->next = NULL;
  if (igrp->firstIseg == NULL) {
    igrp->firstIseg = iseg;
  } else {
    igrp->lastIseg->next = iseg;
  }
  igrp->lastIseg = iseg;
  return iseg;
}


/**************************************************************/

/*
 * global symbol table
 */


#define INITIAL_NUM_BUCKETS	100


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


unsigned char *readObjData(ExecHeader *hdr, unsigned int inOff,
                           FILE *inFile, char *inPath) {
  unsigned char *data;

  data = memAlloc(hdr->sdata);
  if (fseek(inFile, inOff + hdr->odata, SEEK_SET) < 0) {
    error("cannot seek to segment data in input file '%s'", inPath);
  }
  if (fread(data, 1, hdr->sdata, inFile) != hdr->sdata) {
    error("cannot read segment data in input file '%s'", inPath);
  }
  return data;
}


void readObjSegments(Module *mod, unsigned int inOff,
                     FILE *inFile, char *inPath) {
  int i;
  SegmentRecord *seg;

  if (fseek(inFile, inOff, SEEK_SET) < 0) {
    error("cannot seek to segment table in input file '%s'", inPath);
  }
  for (i = 0; i < mod->nsegs; i++) {
    seg = mod->segs + i;
    if (fread(seg, sizeof(SegmentRecord), 1, inFile) != 1) {
      error("cannot read segment record in input file '%s'", inPath);
    }
    conv4FromEcoToNative((unsigned char *) &seg->name);
    conv4FromEcoToNative((unsigned char *) &seg->offs);
    conv4FromEcoToNative((unsigned char *) &seg->addr);
    conv4FromEcoToNative((unsigned char *) &seg->size);
    conv4FromEcoToNative((unsigned char *) &seg->attr);
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
    /* remember a pointer to the symbol in the module's symbol vector */
    /* so that relocations can easily access the symbol by its index */
    mod->syms[i] = sym;
  }
}


void readObjRelocations(Module *mod, unsigned int inOff,
                        FILE *inFile, char *inPath) {
  int i;
  RelocRecord *rel;

  if (fseek(inFile, inOff, SEEK_SET) < 0) {
    error("cannot seek to relocation table in input file '%s'", inPath);
  }
  for (i = 0; i < mod->nrels; i++) {
    rel = mod->rels + i;
    if (fread(rel, sizeof(RelocRecord), 1, inFile) != 1) {
      error("cannot read relocation record in input file '%s'", inPath);
    }
    conv4FromEcoToNative((unsigned char *) &rel->loc);
    conv4FromEcoToNative((unsigned char *) &rel->seg);
    conv4FromEcoToNative((unsigned char *) &rel->typ);
    conv4FromEcoToNative((unsigned char *) &rel->ref);
    conv4FromEcoToNative((unsigned char *) &rel->add);
  }
}


void readObjModule(char *name, unsigned int inOff,
                   FILE *inFile, char *inPath) {
  Module *mod;
  ExecHeader hdr;

  fprintf(stderr,
          "reading module '%s' from file '%s', offset 0x%08X\n",
          name, inPath, inOff);
  mod = newModule(name);
  readObjHeader(&hdr, inOff, inFile, inPath);
  mod->strs = readObjStrings(&hdr, inOff, inFile, inPath);
  mod->data = readObjData(&hdr, inOff, inFile, inPath);
  mod->nsegs = hdr.nsegs;
  mod->segs = memAlloc(hdr.nsegs * sizeof(SegmentRecord));
  readObjSegments(mod, inOff + hdr.osegs, inFile, inPath);
  mod->nsyms = hdr.nsyms;
  mod->syms = memAlloc(hdr.nsyms * sizeof(Sym *));
  readObjSymbols(mod, inOff + hdr.osyms, inFile, inPath);
  mod->nrels = hdr.nrels;
  mod->rels = memAlloc(hdr.nrels * sizeof(RelocRecord));
  readObjRelocations(mod, inOff + hdr.orels, inFile, inPath);
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

  file = firstFile;
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

  mod = firstModule;
  while (mod != NULL) {
    fprintf(stderr, "segments of module '%s':\n", mod->name);
    for (i = 0; i < mod->nsegs; i++) {
      attr[0] = (mod->segs[i].attr & SEG_ATTR_A) ? 'A' : '-';
      attr[1] = (mod->segs[i].attr & SEG_ATTR_P) ? 'P' : '-';
      attr[2] = (mod->segs[i].attr & SEG_ATTR_W) ? 'W' : '-';
      attr[3] = (mod->segs[i].attr & SEG_ATTR_X) ? 'X' : '-';
      attr[4] = '\0';
      fprintf(stderr,
              "    %s : addr = 0x%08X, size = 0x%08X, attr = [%s]\n",
              mod->strs + mod->segs[i].name,
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
  Sym *sym;

  if (strcmp(exp->u.varExp.name, ".") == 0) {
    /* variable is dot */
    return dot;
  } else {
    /* variable is not dot */
    sym = lookupSymbol(exp->u.varExp.name);
    if (sym == NULL) {
      error("symbol '%s' not found", exp->u.varExp.name);
    }
    if (sym->attr & SYM_ATTR_U) {
      error("symbol '%s' is undefined", exp->u.varExp.name);
    }
    if (sym->seg != -1) {
      error("symbol '%s' is not absolute", exp->u.varExp.name);
    }
    return sym->val;
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
  Igrp *igrp;

  igrp = lookupIgrp(stm->u.isegStm.name);
  if (igrp == NULL) {
    /* this should never happen */
    error("iseg group '%s' disappeared",
          stm->u.isegStm.name);
  }
  /* assign start address of group */
  igrp->addr = dot;
  /* update current address with group size */
  dot += igrp->size;
}


void doOsegStm(ScriptNode *stm) {
  Oseg *oseg;
  unsigned int start;

  oseg = findOseg(stm->u.osegStm.name);
  if (oseg == NULL) {
    /* this should never happen */
    error("output segment '%s' disappeared",
          stm->u.osegStm.name);
  }
  oseg->attr = stm->u.osegStm.attr;
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
  NULL, NULL, 0, NULL, 0, NULL, 0, NULL, NULL
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


void buildIgrpTbl(ScriptNode *script) {
  ScriptNode *node1;
  Oseg *oseg;
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
      oseg = newOseg(node1->u.stmList.head->u.osegStm.name);
      node2 = node1->u.stmList.head->u.osegStm.stms;
      while (node2 != NULL) {
        if (node2->u.stmList.head->type == NODE_ISEGSTM) {
          addIgrpToOseg(node2->u.stmList.head->u.isegStm.name,
                        oseg);
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
  Igrp *igrp;

  mod = firstModule;
  while (mod != NULL) {
    for (i = 0; i < mod->nsegs; i++) {
      segName = mod->strs + mod->segs[i].name;
      igrp = lookupIgrp(segName);
      if (igrp == NULL) {
        /* input segment name does not match any ISEG name in script */
        warning("discarding segment '%s' from module '%s'",
                segName, mod->name);
      } else {
        /* set segment's relative address in group */
        mod->segs[i].addr = igrp->size;
        /* add segment's size to group total */
        igrp->size += mod->segs[i].size;
        igrp->size = WORD_ALIGN(igrp->size);
        /* remember input segment as part of this segment group */
        addIsegToIgrp(mod, i, igrp);
      }
    }
    mod = mod->next;
  }
}


void setAbsSegAddrs(void) {
  Module *mod;
  int i;
  char *segName;
  Igrp *igrp;

  mod = firstModule;
  while (mod != NULL) {
    for (i = 0; i < mod->nsegs; i++) {
      segName = mod->strs + mod->segs[i].name;
      igrp = lookupIgrp(segName);
      if (igrp != NULL) {
        /* add group start to segment's relative address */
        mod->segs[i].addr += igrp->addr;
      }
    }
    mod = mod->next;
  }
}


void allocateStorage(ScriptNode *script) {
  /* for each ISEG statement in the linker script
     allocate a group record */
  buildIgrpTbl(script);
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
    fprintf(stderr, "    %s\n", sym->name);
  }
}


void showUndefSymbols(void) {
  mapOverSymbols(showUndefSymbol, NULL);
}


static void showSymbol(Sym *sym, void *arg) {
  Module *mod;

  mod = sym->mod;
  fprintf(stderr,
          "    %s : mod = %s, seg = %s, val = 0x%08X\n",
          sym->name,
          mod->name,
          sym->seg == -1 ?
            "*ABS*" : mod->strs + mod->segs[sym->seg].name,
          sym->val);
}


void showAllSymbols(void) {
  fprintf(stderr, "global symbol table:\n");
  mapOverSymbols(showSymbol, NULL);
}


void resolveSymbol(Sym *sym, void *arg) {
  Module *mod;
  SegmentRecord *seg;

  fprintf(stderr, "    %s = 0x%08X", sym->name, sym->val);
  mod = sym->mod;
  if (sym->seg == -1) {
    /* absolute symbol: keep value */
  } else {
    /* add segment address to symbol value */
    seg = mod->segs + sym->seg;
    sym->val += seg->addr;
  }
  fprintf(stderr,
          " --(%s, %s)--> 0x%08X\n",
          mod->name,
          sym->seg == -1 ? "*ABS*" : mod->strs + seg->name,
          sym->val);
}


void resolveSymbols(void) {
  int n;

  n = countUndefSymbols();
  if (n > 0) {
    fprintf(stderr, "The following symbols are undefined:\n");
    showUndefSymbols();
    error("%d undefined symbol(s)", n);
  }
  fprintf(stderr, "resolving symbols:\n");
  mapOverSymbols(resolveSymbol, NULL);
}


/**************************************************************/


void relocateModules(void) {
  Module *mod;		/* module which is relocated */
  int i;		/* relocation number within the module */
  RelocRecord *rel;	/* pointer to relocation record i */
  SegmentRecord *seg;	/* pointer to segment in which reloc is applied */
  unsigned char *loc;	/* pointer to word within segment's data */
  unsigned int addr;	/* virtual address of word at loc */
  unsigned int data;	/* word at loc, which gets modified */
  unsigned int base;	/* base value of either segment or symbol */
  unsigned int value;	/* base + addend from relocation */
  char *method;		/* relocation method as printable string */
  unsigned int mask;	/* which part of the word gets modified */

  mod = firstModule;
  while (mod != NULL) {
    fprintf(stderr, "relocating module '%s':\n", mod->name);
    for (i = 0; i < mod->nrels; i++) {
      rel = mod->rels + i;
      seg = mod->segs + rel->seg;
      loc = mod->data + seg->offs + rel->loc;
      addr = seg->addr + rel->loc;
      data = read4FromEco(loc);
      fprintf(stderr,
              "    %s @ 0x%08X (vaddr 0x%08X) = 0x%08X\n",
              mod->strs + seg->name, rel->loc, addr, data);
      if (rel->typ & RELOC_SYM) {
        base = mod->syms[rel->ref]->val;
      } else {
        base = mod->segs[rel->ref].addr;
      }
      value = base + rel->add;
      switch (rel->typ & ~RELOC_SYM) {
        case RELOC_H16:
          method = "H16";
          mask = 0x0000FFFF;
          value >>= 16;
          break;
        case RELOC_L16:
          method = "L16";
          mask = 0x0000FFFF;
          break;
        case RELOC_R16:
          method = "R16";
          mask = 0x0000FFFF;
          value -= addr + 4;
          if (value & 3) {
            warning("module %s, segment %s, offset 0x%08X\n"
                    "         "
                    "branch distance is not a multiple of 4",
                    mod->name, mod->strs + seg->name, rel->loc);
          }
          if (((value >> 18) & 0x3FFF) != 0x0000 &&
              ((value >> 18) & 0x3FFF) != 0x3FFF) {
            warning("module %s, segment %s, offset 0x%08X\n"
                    "         "
                    "branch target address out of reach",
                    mod->name, mod->strs + seg->name, rel->loc);
          }
          value >>= 2;
          break;
        case RELOC_R26:
          method = "R26";
          mask = 0x03FFFFFF;
          value -= addr + 4;
          if (value & 3) {
            warning("module %s, segment %s, offset 0x%08X\n"
                    "         "
                    "jump distance is not a multiple of 4",
                    mod->name, mod->strs + seg->name, rel->loc);
          }
          if (((value >> 28) & 0x0F) != 0x00 &&
              ((value >> 28) & 0x0F) != 0x0F) {
            warning("module %s, segment %s, offset 0x%08X\n"
                    "         "
                    "jump target address out of reach",
                    mod->name, mod->strs + seg->name, rel->loc);
          }
          value >>= 2;
          break;
        case RELOC_W32:
          method = "W32";
          mask = 0xFFFFFFFF;
          break;
        default:
          method = "ILL";
          mask = 0;
          error("illegal relocation type %d", rel->typ & ~RELOC_SYM);
      }
      data = (data & ~mask) | (value & mask);
      write4ToEco(loc, data);
      fprintf(stderr,
              "        --(%s, %s %s, 0x%08X)--> 0x%08X\n",
              method,
              rel->typ & RELOC_SYM ? "SYM" : "SEG",
              rel->typ & RELOC_SYM ?
                mod->syms[rel->ref]->name :
                mod->strs + mod->segs[rel->ref].name,
              rel->add,
              data);
    }
    mod = mod->next;
  }
}


/**************************************************************/


static ExecHeader execHeader;

static unsigned int outFileOffset;
static unsigned int outDataSize;
static unsigned int outStringSize;


static void writeDummyHeader(FILE *outFile) {
  if (fwrite(&execHeader, sizeof(ExecHeader), 1, outFile) != 1) {
    error("cannot write output file header");
  }
  /* update file offset */
  outFileOffset += sizeof(ExecHeader);
}


static void writeRealHeader(FILE *outFile) {
  rewind(outFile);
  execHeader.magic = EXEC_MAGIC;
  execHeader.entry = (entry == NULL) ? 0 : entry->val;
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
  if (fwrite(&execHeader, sizeof(ExecHeader), 1, outFile) != 1) {
    error("cannot write output file header");
  }
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


static void writeData(FILE *outFile) {
  Oseg *oseg;
  Igrp *igrp;
  Iseg *iseg;
  Module *mod;
  SegmentRecord *seg;
  unsigned char *data;
  unsigned int size;

  execHeader.odata = outFileOffset;
  execHeader.sdata = 0;
  oseg = firstOseg;
  while (oseg != NULL) {
    oseg->dataOffs = execHeader.sdata;
    if (oseg->attr & SEG_ATTR_P) {
      igrp = oseg->firstIgrp;
      while (igrp != NULL) {
        iseg = igrp->firstIseg;
        while (iseg != NULL) {
          mod = iseg->mod;
          seg = mod->segs + iseg->seg;
          data = mod->data + seg->offs;
          size = seg->size;
          if (size != 0) {
            if (fwrite(data, 1, size, outFile) != size) {
              error("cannot write output file segment data");
            }
          }
          execHeader.sdata += size;
          iseg = iseg->next;
        }
        igrp = igrp->next;
      }
    }
    oseg = oseg->next;
  }
  /* update file offset */
  outFileOffset += execHeader.sdata;
}


static void writeStrings(FILE *outFile) {
  Oseg *oseg;
  unsigned int n;

  execHeader.ostrs = outFileOffset;
  execHeader.sstrs = 0;
  oseg = firstOseg;
  while (oseg != NULL) {
    oseg->nameOffs = execHeader.sstrs;
    n = strlen(oseg->name) + 1;
    if (fwrite(oseg->name, 1, n, outFile) != n) {
      error("cannot write output file strings");
    }
    execHeader.sstrs += n;
    oseg = oseg->next;
  }
  /* update file offset */
  outFileOffset += execHeader.sstrs;
}


static void writeSegment(Oseg *oseg, FILE *outFile) {
  SegmentRecord segment;

  segment.name = oseg->nameOffs;
  segment.offs = oseg->dataOffs;
  segment.addr = oseg->addr;
  segment.size = oseg->size;
  segment.attr = oseg->attr;
  conv4FromNativeToEco((unsigned char *) &segment.name);
  conv4FromNativeToEco((unsigned char *) &segment.offs);
  conv4FromNativeToEco((unsigned char *) &segment.addr);
  conv4FromNativeToEco((unsigned char *) &segment.size);
  conv4FromNativeToEco((unsigned char *) &segment.attr);
  if (fwrite(&segment, sizeof(SegmentRecord), 1, outFile) != 1) {
    error("cannot write output file segment");
  }
  /* update file offset */
  outFileOffset += sizeof(SegmentRecord);
}


static void writeSegmentTable(FILE *outFile) {
  Oseg *oseg;

  execHeader.osegs = outFileOffset;
  execHeader.nsegs = 0;
  oseg = firstOseg;
  while (oseg != NULL) {
    writeSegment(oseg, outFile);
    execHeader.nsegs++;
    oseg = oseg->next;
  }
}


void writeOutput(char *outName) {
  FILE *outFile;

  showAllOsegs(outName);
  outFile = fopen(outName, "w");
  if (outFile == NULL) {
    error("cannot open output file '%s'", outName);
  }
  outFileOffset = 0;
  outDataSize = 0;
  outStringSize = 0;
  writeDummyHeader(outFile);
  writeData(outFile);
  writeStrings(outFile);
  writeSegmentTable(outFile);
  writeRealHeader(outFile);
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
    fprintf(stderr, "arg %d: %s\n", i, argv[i]);
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
  if (firstFile == NULL) {
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
  writeOutput(outName);
  if (mapName != NULL) {
    writeMap(mapName);
  }
  return 0;
}
