/*
 * ld.c -- ECO32 link editor
 */


/*
 * This is the solution to exercise 5:
 *   - read a list of object files
 *   - build the global symbol table
 *   - some of the files may be static libraries
 *   - decide whether a module must be extracted
 *   - allocate memory for the modules
 *   - resolve symbols
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "eof.h"
#include "ar.h"


/**************************************************************/

/*
 * macros
 */


#define DEFAULT_CODE_BASE	0x1000
#define DEFAULT_START_SYMBOL	"_start"
#define DEFAULT_END_SYMBOL	"_end"
#define DEFAULT_OUT_NAME	"a.out"

#define WORD_ALIGN(x)		(((x) + 0x03) & ~0x03)
#define PAGE_ALIGN(x)		(((x) + 0x0FFF) & ~0x0FFF)


/**************************************************************/

/*
 * debugging
 */


int debugExtract = 1;
int debugSegments = 1;
int debugGroups = 1;
int debugResolve = 1;


/**************************************************************/

/*
 * error handling, memory allocation
 */


void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  fprintf(stderr, "error: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
  exit(1);
}


void warning(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  fprintf(stderr, "warning: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}


void *memAlloc(unsigned int size) {
  void *p;

  p = malloc(size);
  if (p == NULL) {
    error("out of memory");
  }
  return p;
}


void memFree(void *p) {
  if (p == NULL) {
    error("memFree() got NULL pointer");
  }
  free(p);
}


/**************************************************************/

/*
 * endianness conversion
 */


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

/*
 * data structures
 */


typedef struct module {
  char *name;			/* module name */
  unsigned char *data;		/* data space */
  char *strs;			/* string space */
  int nsegs;			/* number of segments */
  struct segment *segs;		/* array of segments */
  int nsyms;			/* number of symbols */
  struct symbol **syms;		/* array of pointers to symbols */
  int nrels;			/* number of relocations */
  struct reloc *rels;		/* array of relocations */
  struct module *next;		/* next module, order is important */
} Module;


Module *firstModule = NULL;
Module *lastModule;


Module *newModule(char *name) {
  Module *mod;

  mod = memAlloc(sizeof(Module));
  mod->name = name;
  mod->data = NULL;
  mod->strs = NULL;
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


/*
 * This is a dummy module record in order to have
 * a home for linker-defined symbols etc.
 */
Module linkerModule = {
  "linker",
  NULL, NULL,
  0, NULL,
  0, NULL,
  0, NULL,
  NULL
};


typedef struct segment {
  char *name;			/* segment name */
  unsigned char *data;		/* segment data */
  unsigned int addr;		/* virtual start address */
  unsigned int size;		/* size of segment in bytes */
  unsigned int npad;		/* number of padding bytes */
  unsigned int attr;		/* segment attributes */
} Segment;


typedef struct symbol {
  /* symbol data */
  char *name;			/* the symbol's name */
  Module *mod;			/* the symbol is defined there */
  int seg;			/* within this segment (-1 for absolute) */
  int val;			/* the symbol's value */
  unsigned int attr;		/* the symbol's attributes */
  /* internal data */
  unsigned int hash;		/* full hash over symbol's name */
  struct symbol *next;		/* hash bucket chain */
} Symbol;


typedef struct reloc {
  unsigned int loc;		/* where to relocate */
  int seg;			/* in which segment */
  int typ;			/* relocation type: one of RELOC_xxx */
				/* symbol flag RELOC_SYM may be set */
  int ref;			/* what is referenced */
				/* if symbol flag = 0: segment number */
				/* if symbol flag = 1: symbol number */
  int add;			/* additive part of value */
} Reloc;


/**************************************************************/

/*
 * global symbol table
 */


#define INITIAL_NUM_BUCKETS	100


static Symbol **buckets = NULL;
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
  buckets = (Symbol **) memAlloc(numBuckets * sizeof(Symbol *));
  for (i = 0; i < numBuckets; i++) {
    buckets[i] = NULL;
  }
  numEntries = 0;
}


static void growTable(void) {
  int newNumBuckets;
  Symbol **newBuckets;
  int i, n;
  Symbol *p, *q;

  /* compute new hash size */
  newNumBuckets = 2 * numBuckets + 1;
  while (!isPrime(newNumBuckets)) {
    newNumBuckets += 2;
  }
  /* init new hash table */
  newBuckets = (Symbol **) memAlloc(newNumBuckets * sizeof(Symbol *));
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


Symbol *lookupSymbol(char *name) {
  unsigned int h;
  int n;
  Symbol *p;

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


Symbol *enterSymbol(char *name) {
  unsigned int h;
  int n;
  Symbol *p;

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
    /* this changed numBuckets, thus n is now invalid */
    n = h % numBuckets;
  }
  /* allocate new sym and link to bucket list */
  p = (Symbol *) memAlloc(sizeof(Symbol));
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


void mapOverSymbols(void (*fp)(Symbol *sym, void *arg), void *arg) {
  int i;
  Symbol *sym;

  for (i = 0; i < numBuckets; i++) {
    sym = buckets[i];
    while (sym != NULL) {
      (*fp)(sym, arg);
      sym = sym->next;
    }
  }
}


int numberOfSymbols(void) {
  return numEntries;
}


/**************************************************************/

/*
 * object module reader
 */


void readObjHeader(EofHeader *hdr, unsigned int ohdr,
                   FILE *inFile, char *inPath) {
  if (fseek(inFile, ohdr, SEEK_SET) < 0) {
    error("cannot seek to header in input file '%s'", inPath);
  }
  if (fread(hdr, sizeof(EofHeader), 1, inFile) != 1) {
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
  if (hdr->magic != EOF_MAGIC) {
    error("wrong magic number in input file '%s'", inPath);
  }
}


void readObjData(Module *mod,
                 unsigned int odata,
                 unsigned int sdata,
                 FILE *inFile, char *inPath) {
  mod->data = memAlloc(sdata);
  if (fseek(inFile, odata, SEEK_SET) < 0) {
    error("cannot seek to segment data in input file '%s'", inPath);
  }
  if (fread(mod->data, 1, sdata, inFile) != sdata) {
    error("cannot read segment data in input file '%s'", inPath);
  }
}


void readObjStrings(Module *mod,
                    unsigned int ostrs,
                    unsigned int sstrs,
                    FILE *inFile, char *inPath) {
  mod->strs = memAlloc(sstrs);
  if (fseek(inFile, ostrs, SEEK_SET) < 0) {
    error("cannot seek to strings in input file '%s'", inPath);
  }
  if (fread(mod->strs, 1, sstrs, inFile) != sstrs) {
    error("cannot read strings in input file '%s'", inPath);
  }
}


void readObjSegments(Module *mod,
                     unsigned int osegs,
                     unsigned int nsegs,
                     FILE *inFile, char *inPath) {
  int i;
  Segment *seg;
  SegmentRecord segRec;

  mod->nsegs = nsegs;
  mod->segs = memAlloc(nsegs * sizeof(Segment));
  if (fseek(inFile, osegs, SEEK_SET) < 0) {
    error("cannot seek to segment table in input file '%s'", inPath);
  }
  for (i = 0; i < nsegs; i++) {
    seg = mod->segs + i;
    if (fread(&segRec, sizeof(SegmentRecord), 1, inFile) != 1) {
      error("cannot read segment record in input file '%s'", inPath);
    }
    conv4FromEcoToNative((unsigned char *) &segRec.name);
    conv4FromEcoToNative((unsigned char *) &segRec.offs);
    conv4FromEcoToNative((unsigned char *) &segRec.addr);
    conv4FromEcoToNative((unsigned char *) &segRec.size);
    conv4FromEcoToNative((unsigned char *) &segRec.attr);
    seg->name = mod->strs + segRec.name;
    seg->data = mod->data + segRec.offs;
    seg->addr = segRec.addr;
    seg->size = segRec.size;
    seg->npad = 0;
    seg->attr = segRec.attr;
  }
}


void readObjSymbols(Module *mod,
                    unsigned int osyms,
                    unsigned int nsyms,
                    FILE *inFile, char *inPath) {
  int i;
  SymbolRecord symRec;
  Symbol *sym;

  mod->nsyms = nsyms;
  mod->syms = memAlloc(nsyms * sizeof(Symbol *));
  if (fseek(inFile, osyms, SEEK_SET) < 0) {
    error("cannot seek to symbol table in input file '%s'", inPath);
  }
  for (i = 0; i < nsyms; i++) {
    if (fread(&symRec, sizeof(SymbolRecord), 1, inFile) != 1) {
      error("cannot read symbol record in input file '%s'", inPath);
    }
    conv4FromEcoToNative((unsigned char *) &symRec.name);
    conv4FromEcoToNative((unsigned char *) &symRec.val);
    conv4FromEcoToNative((unsigned char *) &symRec.seg);
    conv4FromEcoToNative((unsigned char *) &symRec.attr);
    sym = enterSymbol(mod->strs + symRec.name);
    if ((symRec.attr & SYM_ATTR_U) == 0) {
      /* symbol gets defined here */
      if ((sym->attr & SYM_ATTR_U) == 0) {
        /* but is already defined in table */
        error("symbol '%s' in module '%s' defined more than once\n"
              "       (previous definition is in module '%s')",
              sym->name, mod->name, sym->mod->name);
      }
      /* copy symbol information to table */
      sym->mod = mod;
      sym->seg = symRec.seg;
      sym->val = symRec.val;
      sym->attr = symRec.attr;
    }
    /* remember a pointer to the symbol in the module's symbol vector */
    /* so that relocations can easily access the symbol by its index */
    mod->syms[i] = sym;
  }
}


void readObjRelocs(Module *mod,
                   unsigned int orels,
                   unsigned int nrels,
                   FILE *inFile, char *inPath) {
  int i;
  Reloc *rel;
  RelocRecord relRec;

  mod->nrels = nrels;
  mod->rels = memAlloc(nrels * sizeof(Reloc));
  if (fseek(inFile, orels, SEEK_SET) < 0) {
    error("cannot seek to relocation table in input file '%s'", inPath);
  }
  for (i = 0; i < nrels; i++) {
    rel = mod->rels + i;
    if (fread(&relRec, sizeof(RelocRecord), 1, inFile) != 1) {
      error("cannot read relocation record in input file '%s'", inPath);
    }
    conv4FromEcoToNative((unsigned char *) &relRec.loc);
    conv4FromEcoToNative((unsigned char *) &relRec.seg);
    conv4FromEcoToNative((unsigned char *) &relRec.typ);
    conv4FromEcoToNative((unsigned char *) &relRec.ref);
    conv4FromEcoToNative((unsigned char *) &relRec.add);
    rel->loc = relRec.loc;
    rel->seg = relRec.seg;
    rel->typ = relRec.typ;
    rel->ref = relRec.ref;
    rel->add = relRec.add;
  }
}


Module *readObjModule(char *name, unsigned int ohdr,
                      FILE *inFile, char *inPath) {
  EofHeader hdr;
  Module *mod;

  mod = newModule(name);
  readObjHeader(&hdr, ohdr, inFile, inPath);
  readObjData(mod, ohdr + hdr.odata, hdr.sdata, inFile, inPath);
  readObjStrings(mod, ohdr + hdr.ostrs, hdr.sstrs, inFile, inPath);
  readObjSegments(mod, ohdr + hdr.osegs, hdr.nsegs, inFile, inPath);
  readObjSymbols(mod, ohdr + hdr.osyms, hdr.nsyms, inFile, inPath);
  readObjRelocs(mod, ohdr + hdr.orels, hdr.nrels, inFile, inPath);
  return mod;
}


/**************************************************************/

/*
 * archive reader
 */


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


char *readArchStrings(unsigned int ostrs,
                      unsigned int sstrs,
                      FILE *inFile, char *inPath) {
  char *strs;

  strs = memAlloc(sstrs);
  if (fseek(inFile, ostrs, SEEK_SET) < 0) {
    error("cannot seek to strings in input file '%s'", inPath);
  }
  if (fread(strs, 1, sstrs, inFile) != sstrs) {
    error("cannot read strings in input file '%s'", inPath);
  }
  return strs;
}


ModuleRecord *readArchModules(unsigned int omods,
                              unsigned int nmods,
                              FILE *inFile, char *inPath) {
  ModuleRecord *mods;
  int i;

  mods = memAlloc(nmods * sizeof(ModuleRecord));
  if (fseek(inFile, omods, SEEK_SET) < 0) {
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
  return mods;
}


int needArchModule(char *fsym, unsigned int nsym) {
  int i;
  Symbol *sym;

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
  strs = readArchStrings(hdr.ostrs, hdr.sstrs, inFile, inPath);
  mods = readArchModules(hdr.omods, hdr.nmods, inFile, inPath);
  /* iterate over library as long as modules get extracted */
  do {
    anyModuleExtracted = 0;
    for (i = 0; i < hdr.nmods; i++) {
      if (needArchModule(strs + mods[i].fsym, mods[i].nsym)) {
        if (debugExtract) {
          fprintf(stderr, "extracting module '%s' from library '%s'\n",
                  strs + mods[i].name, inPath);
        }
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

/*
 * input file reader
 */


typedef struct file {
  char *path;
  struct file *next;
} File;


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


int noFiles(void) {
  return firstFile == NULL;
}


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
    if (magic == EOF_MAGIC) {
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
      error("input file '%s' is not an object file", file->path);
    }
    fclose(inFile);
    file = file->next;
  }
}


/**************************************************************/

/*
 * segment viewer
 */


void showSegment(Segment *seg) {
  char attr[10];

  attr[0] = (seg->attr & SEG_ATTR_A) ? 'A' : '-';
  attr[1] = (seg->attr & SEG_ATTR_P) ? 'P' : '-';
  attr[2] = (seg->attr & SEG_ATTR_W) ? 'W' : '-';
  attr[3] = (seg->attr & SEG_ATTR_X) ? 'X' : '-';
  attr[4] = '\0';
  fprintf(stderr,
          "    %s : addr = 0x%08X, size = 0x%08X + %u, attr = [%s]\n",
          seg->name, seg->addr, seg->size, seg->npad, attr);
}


void showSegments(void) {
  Module *mod;
  Segment *seg;
  int i;

  mod = firstModule;
  while (mod != NULL) {
    fprintf(stderr, "segments in module '%s':\n", mod->name);
    for (i = 0; i < mod->nsegs; i++) {
      seg = mod->segs + i;
      showSegment(seg);
    }
    mod = mod->next;
  }
}


/**************************************************************/

/*
 * storage allocator
 */


#define SEG_ATTR_APX	(SEG_ATTR_A | SEG_ATTR_P | SEG_ATTR_X)
#define SEG_ATTR_APW	(SEG_ATTR_A | SEG_ATTR_P | SEG_ATTR_W)
#define SEG_ATTR_AW	(SEG_ATTR_A | SEG_ATTR_W)


typedef struct partialSegment {
  Module *mod;				/* module where part comes from */
  Segment *seg;				/* which segment in the module */
  struct partialSegment *next;		/* next part in total segment */
} PartialSegment;


typedef struct totalSegment {
  char *name;				/* name of total segment */
  unsigned int nameOffs;		/* name offset in string space */
  unsigned int dataOffs;		/* data offset in data space */
  unsigned int addr;			/* virtual address */
  unsigned int size;			/* size in bytes */
  unsigned int attr;			/* attributes */
  struct partialSegment *firstPart;	/* first partial segment in total */
  struct partialSegment *lastPart;	/* last partial segment in total */
  struct totalSegment *next;		/* next total in segment group */
} TotalSegment;


typedef struct segmentGroup {
  unsigned int attr;			/* attributes of this group */
  TotalSegment *firstTotal;		/* first total segment in group */
  TotalSegment *lastTotal;		/* last total segment in group */
} SegmentGroup;


SegmentGroup groupAPX = { SEG_ATTR_APX, NULL, NULL };
SegmentGroup groupAPW = { SEG_ATTR_APW, NULL, NULL };
SegmentGroup groupAW  = { SEG_ATTR_AW,  NULL, NULL };


void addToGroup(Module *mod, Segment *seg, SegmentGroup *grp) {
  TotalSegment *total;
  PartialSegment *part;

  total = grp->firstTotal;
  while (total != NULL) {
    if (strcmp(seg->name, total->name) == 0) {
      break;
    }
    total = total->next;
  }
  if (total == NULL) {
    total = memAlloc(sizeof(TotalSegment));
    total->name = seg->name;
    total->nameOffs = 0;
    total->dataOffs = 0;
    total->addr = 0;
    total->size = 0;
    total->attr = grp->attr;
    total->firstPart = NULL;
    total->lastPart = NULL;
    total->next = NULL;
    if (grp->firstTotal == NULL) {
      grp->firstTotal = total;
    } else {
      grp->lastTotal->next = total;
    }
    grp->lastTotal = total;
  }
  part = memAlloc(sizeof(PartialSegment));
  part->mod = mod;
  part->seg = seg;
  part->next = NULL;
  if (total->firstPart == NULL) {
    total->firstPart = part;
  } else {
    total->lastPart->next = part;
  }
  total->lastPart = part;
}


unsigned int setPartAddrs(PartialSegment *part, unsigned int addr) {
  unsigned int size;
  Segment *seg;
  unsigned int n;

  size = 0;
  while (part != NULL) {
    seg = part->seg;
    seg->addr = addr;
    n = WORD_ALIGN(seg->size);
    seg->npad = n - seg->size;
    addr += n;
    size += n;
    part = part->next;
  }
  return size;
}


unsigned int setTotalAddrs(TotalSegment *total, unsigned int addr) {
  unsigned int size;
  unsigned int n;

  size = 0;
  while (total != NULL) {
    n = setPartAddrs(total->firstPart, addr);
    total->addr = addr;
    total->size = n;
    addr += n;
    size += n;
    total = total->next;
  }
  return size;
}


void allocateStorage(unsigned int codeBase,
                     int dataPageAlign,
                     char *endSymbol) {
  Module *mod;
  Segment *seg;
  int i;
  unsigned int addr;
  unsigned int size;
  Symbol *endSym;

  /* pass 1: combine segments from modules in three groups */
  mod = firstModule;
  while (mod != NULL) {
    for (i = 0; i < mod->nsegs; i++) {
      seg = mod->segs + i;
      if (seg->attr == SEG_ATTR_APX) {
        addToGroup(mod, seg, &groupAPX);
      } else
      if (seg->attr == SEG_ATTR_APW) {
        addToGroup(mod, seg, &groupAPW);
      } else
      if (seg->attr == SEG_ATTR_AW) {
        addToGroup(mod, seg, &groupAW);
      } else {
        warning("segment '%s' in module '%s' discarded",
                seg->name, mod->name);
      }
    }
    mod = mod->next;
  }
  /* pass 2: compute sum of sizes and set start addresses */
  addr = codeBase;
  size = setTotalAddrs(groupAPX.firstTotal, addr);
  addr += size;
  if (dataPageAlign) {
    addr = PAGE_ALIGN(addr);
  }
  size = setTotalAddrs(groupAPW.firstTotal, addr);
  addr += size;
  size = setTotalAddrs(groupAW.firstTotal, addr);
  addr += size;
  endSym = enterSymbol(endSymbol);
  endSym->mod = &linkerModule;
  endSym->seg = -1;
  endSym->val = addr;
  endSym->attr &= ~SYM_ATTR_U;
}


/**************************************************************/

/*
 * segment group viewer
 */


void showParts(PartialSegment *part) {
  Module *mod;
  Segment *seg;

  while (part != NULL) {
    mod = part->mod;
    seg = part->seg;
    fprintf(stderr,
            "        addr = 0x%08X, size = 0x%08X + %u (module '%s')\n",
            seg->addr, seg->size, seg->npad, mod->name);
    part = part->next;
  }
}


void showTotals(TotalSegment *total) {
  while (total != NULL) {
    fprintf(stderr,
            "    segment '%s' (addr = 0x%08X, size = 0x%08X):\n",
            total->name, total->addr, total->size);
    showParts(total->firstPart);
    total = total->next;
  }
}


void showGroups(void) {
  fprintf(stderr, "segments in group [AP-X]:\n");
  showTotals(groupAPX.firstTotal);
  fprintf(stderr, "segments in group [APW-]:\n");
  showTotals(groupAPW.firstTotal);
  fprintf(stderr, "segments in group [A-W-]:\n");
  showTotals(groupAW.firstTotal);
}


/**************************************************************/

/*
 * symbol resolution
 */


static void countUndefSymbol(Symbol *sym, void *arg) {
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


static void showUndefSymbol(Symbol *sym, void *arg) {
  if (sym->attr & SYM_ATTR_U) {
    fprintf(stderr, "    %s\n", sym->name);
  }
}


void showUndefSymbols(void) {
  mapOverSymbols(showUndefSymbol, NULL);
}


static void resolveSymbol(Symbol *sym, void *arg) {
  Module *mod;
  Segment *seg;

  if (debugResolve) {
    fprintf(stderr, "    %s = 0x%08X", sym->name, sym->val);
  }
  mod = sym->mod;
  if (sym->seg == -1) {
    /* absolute symbol: keep value */
  } else {
    /* add segment address to symbol value */
    seg = mod->segs + sym->seg;
    sym->val += seg->addr;
  }
  if (debugResolve) {
    fprintf(stderr,
            " --(%s, %s)--> 0x%08X\n",
            mod->name,
            sym->seg == -1 ? "*ABS*" : seg->name,
            sym->val);
  }
}


void resolveSymbols(void) {
  int n;

  n = countUndefSymbols();
  if (n > 0) {
    fprintf(stderr, "The following symbols are undefined:\n");
    showUndefSymbols();
    error("%d undefined symbol(s)", n);
  }
  if (debugResolve) {
    fprintf(stderr, "resolving symbols:\n");
  }
  mapOverSymbols(resolveSymbol, NULL);
}


/**************************************************************/

/*
 * symbol table writer
 */


typedef struct {
  Symbol **symbolTable;
  int currentIndex;
} SymtableIterator;


static void getSymbol(Symbol *sym, void *arg) {
  SymtableIterator *iter;

  iter = (SymtableIterator *) arg;
  iter->symbolTable[iter->currentIndex++] = sym;
}


static int compareSymbols(const void *p1, const void *p2) {
  Symbol **sym1;
  Symbol **sym2;

  sym1 = (Symbol **) p1;
  sym2 = (Symbol **) p2;
  if ((*sym1)->val < (*sym2)->val) {
    return -1;
  }
  if ((*sym1)->val > (*sym2)->val) {
    return 1;
  }
  return strcmp((*sym1)->name, (*sym2)->name);
}


void writeSymbolTable(FILE *mapFile) {
  int numSymbols;
  SymtableIterator iter;
  int i;
  Symbol *sym;
  Module *mod;

  numSymbols = numberOfSymbols();
  iter.symbolTable = memAlloc(numSymbols * sizeof(Symbol *));
  iter.currentIndex = 0;
  mapOverSymbols(getSymbol, &iter);
  qsort(iter.symbolTable, numSymbols, sizeof(Symbol *), compareSymbols);
  for (i = 0; i < numSymbols; i++) {
    sym = iter.symbolTable[i];
    mod = sym->mod;
    fprintf(mapFile,
            "%-24s  0x%08X  %-12s  %s\n",
            sym->name,
            sym->val,
            sym->seg == -1 ?
              "*ABS*" : mod->segs[sym->seg].name,
            mod->name);
  }
  memFree(iter.symbolTable);
}


/**************************************************************/

/*
 * output file writer
 */


static EofHeader outFileHeader;
static unsigned int outFileOffset;


static void writeDummyHeader(FILE *outFile) {
  memset(&outFileHeader, 0, sizeof(EofHeader));
  if (fwrite(&outFileHeader, sizeof(EofHeader), 1, outFile) != 1) {
    error("cannot write output file header");
  }
  /* init file offset */
  outFileOffset = sizeof(EofHeader);
}


static void writeFinalHeader(unsigned int entry, FILE *outFile) {
  outFileHeader.magic = EOF_MAGIC;
  outFileHeader.entry = entry;
  conv4FromNativeToEco((unsigned char *) &outFileHeader.magic);
  conv4FromNativeToEco((unsigned char *) &outFileHeader.osegs);
  conv4FromNativeToEco((unsigned char *) &outFileHeader.nsegs);
  conv4FromNativeToEco((unsigned char *) &outFileHeader.osyms);
  conv4FromNativeToEco((unsigned char *) &outFileHeader.nsyms);
  conv4FromNativeToEco((unsigned char *) &outFileHeader.orels);
  conv4FromNativeToEco((unsigned char *) &outFileHeader.nrels);
  conv4FromNativeToEco((unsigned char *) &outFileHeader.odata);
  conv4FromNativeToEco((unsigned char *) &outFileHeader.sdata);
  conv4FromNativeToEco((unsigned char *) &outFileHeader.ostrs);
  conv4FromNativeToEco((unsigned char *) &outFileHeader.sstrs);
  conv4FromNativeToEco((unsigned char *) &outFileHeader.entry);
  if (fseek(outFile, 0, SEEK_SET) < 0) {
    error("cannot seek to header in output file");
  }
  if (fwrite(&outFileHeader, sizeof(EofHeader), 1, outFile) != 1) {
    error("cannot write output file header");
  }
}


void writeDataForParts(PartialSegment *part, FILE *outFile) {
  Segment *seg;
  unsigned int size;
  unsigned int npad;
  static unsigned char pad[3] = { 0, 0, 0 };

  while (part != NULL) {
    seg = part->seg;
    size = seg->size;
    if (size != 0) {
      if (fwrite(seg->data, 1, size, outFile) != size) {
        error("cannot write output file segment data");
      }
      npad = seg->npad;
      if (npad != 0) {
        if (fwrite(pad, 1, npad, outFile) != npad) {
          error("cannot write output file segment data");
        }
      }
      outFileHeader.sdata += size + npad;
    }
    part = part->next;
  }
}


void writeDataForTotals(TotalSegment *total, FILE *outFile) {
  while (total != NULL) {
    total->dataOffs = outFileHeader.sdata;
    writeDataForParts(total->firstPart, outFile);
    total = total->next;
  }
}


void writeDataForGroups(FILE *outFile) {
  outFileHeader.odata = outFileOffset;
  outFileHeader.sdata = 0;
  writeDataForTotals(groupAPX.firstTotal, outFile);
  writeDataForTotals(groupAPW.firstTotal, outFile);
  /* update file offset */
  outFileOffset += outFileHeader.sdata;
}


void writeStringsForTotals(TotalSegment *total, FILE *outFile) {
  unsigned int size;

  while (total != NULL) {
    total->nameOffs = outFileHeader.sstrs;
    size = strlen(total->name) + 1;
    if (fwrite(total->name, 1, size, outFile) != size) {
      error("cannot write output file strings");
    }
    outFileHeader.sstrs += size;
    total = total->next;
  }
}


void writeStringsForGroups(FILE *outFile) {
  outFileHeader.ostrs = outFileOffset;
  outFileHeader.sstrs = 0;
  writeStringsForTotals(groupAPX.firstTotal, outFile);
  writeStringsForTotals(groupAPW.firstTotal, outFile);
  writeStringsForTotals(groupAW.firstTotal, outFile);
  /* update file offset */
  outFileOffset += outFileHeader.sstrs;
}


void writeSegment(TotalSegment *total, FILE *outFile) {
  SegmentRecord segRec;

  segRec.name = total->nameOffs;
  segRec.offs = total->dataOffs;
  segRec.addr = total->addr;
  segRec.size = total->size;
  segRec.attr = total->attr;
  conv4FromNativeToEco((unsigned char *) &segRec.name);
  conv4FromNativeToEco((unsigned char *) &segRec.offs);
  conv4FromNativeToEco((unsigned char *) &segRec.addr);
  conv4FromNativeToEco((unsigned char *) &segRec.size);
  conv4FromNativeToEco((unsigned char *) &segRec.attr);
  if (fwrite(&segRec, sizeof(SegmentRecord), 1, outFile) != 1) {
    error("cannot write output file segment");
  }
  /* update file offset */
  outFileOffset += sizeof(SegmentRecord);
}


void writeSegmentForTotals(TotalSegment *total, FILE *outFile) {
  while (total != NULL) {
    writeSegment(total, outFile);
    outFileHeader.nsegs++;
    total = total->next;
  }
}


void writeSegmentsForGroups(FILE *outFile) {
  outFileHeader.osegs = outFileOffset;
  outFileHeader.nsegs = 0;
  writeSegmentForTotals(groupAPX.firstTotal, outFile);
  writeSegmentForTotals(groupAPW.firstTotal, outFile);
  writeSegmentForTotals(groupAW.firstTotal, outFile);
}


void writeObjModule(char *startSymbol, char *outPath) {
  Symbol *startSym;
  unsigned int entry;
  FILE *outFile;

  startSym = lookupSymbol(startSymbol);
  if (startSym == NULL) {
    error("undefined start symbol '%s'", startSymbol);
  }
  entry = startSym->val;
  outFile = fopen(outPath, "w");
  if (outFile == NULL) {
    error("cannot open output file '%s'", outPath);
  }
  writeDummyHeader(outFile);
  writeDataForGroups(outFile);
  writeStringsForGroups(outFile);
  writeSegmentsForGroups(outFile);
  writeFinalHeader(entry, outFile);
  fclose(outFile);
}


/**************************************************************/

/*
 * map file writer
 */


void writeMap(char *mapName) {
  FILE *mapFile;

  mapFile = fopen(mapName, "w");
  if (mapFile == NULL) {
    error("cannot open map file '%s'", mapName);
  }
  writeSymbolTable(mapFile);
  fclose(mapFile);
}


/**************************************************************/

/*
 * main program
 */


void usage(char *myself) {
  fprintf(stderr, "usage: %s\n", myself);
  fprintf(stderr, "         [-d]             data directly after code\n");
  fprintf(stderr, "         [-c <addr>]      set code base address\n");
  fprintf(stderr, "         [-s <symbol>]    set code start symbol\n");
  fprintf(stderr, "         [-e <symbol>]    set bss end symbol\n");
  fprintf(stderr, "         [-o <outfile>]   set output file name\n");
  fprintf(stderr, "         [-m <mapfile>]   set map file name\n");
  fprintf(stderr, "         <infile> ...     input file names\n");
  exit(1);
}


int main(int argc, char *argv[]) {
  int i;
  int dataPageAlign;
  unsigned int codeBase;
  char *startSymbol;
  char *endSymbol;
  char *outName;
  char *mapName;
  char *endptr;

  dataPageAlign = 1;
  codeBase = DEFAULT_CODE_BASE;
  startSymbol = DEFAULT_START_SYMBOL;
  endSymbol = DEFAULT_END_SYMBOL;
  outName = DEFAULT_OUT_NAME;
  mapName = NULL;
  for (i = 1; i < argc; i++) {
    if (*argv[i] == '-') {
      /* option */
      if (strcmp(argv[i], "-d") == 0) {
        dataPageAlign = 0;
      } else
      if (strcmp(argv[i], "-c") == 0) {
        if (++i == argc) {
          error("option '-c' is missing an address");
        }
        codeBase = strtoul(argv[i], &endptr, 0);
        if (*endptr != '\0') {
          error("option '-c' has an invalid address");
        }
        if (WORD_ALIGN(codeBase) != codeBase) {
          warning("code base address is not word-aligned");
        }
      } else
      if (strcmp(argv[i], "-s") == 0) {
        if (++i == argc) {
          error("option '-s' is missing a symbol name");
        }
        startSymbol = argv[i];
      } else
      if (strcmp(argv[i], "-e") == 0) {
        if (++i == argc) {
          error("option '-e' is missing a symbol name");
        }
        endSymbol = argv[i];
      } else
      if (strcmp(argv[i], "-o") == 0) {
        if (++i == argc) {
          error("option '-o' is missing a file name");
        }
        outName = argv[i];
      } else
      if (strcmp(argv[i], "-m") == 0) {
        if (++i == argc) {
          error("option '-m' is missing a file name");
        }
        mapName = argv[i];
      } else {
        usage(argv[0]);
      }
    } else {
      /* input file */
      newFile(argv[i]);
    }
  }
  if (noFiles()) {
    error("no input files");
  }
  readFiles();
  if (debugSegments) {
    showSegments();
  }
  allocateStorage(codeBase, dataPageAlign, endSymbol);
  if (debugGroups) {
    showGroups();
  }
  resolveSymbols();
  writeObjModule(startSymbol, outName);
  if (mapName != NULL) {
    writeMap(mapName);
  }
  return 0;
}
