/*
 * load.c -- ECO32 loader
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "eof.h"


/**************************************************************/


#define MAX_MEMSIZE		512	/* maximum memory size in MB */
#define DEFAULT_MEMSIZE		4	/* default memory size in MB */

#define PAGE_ALIGN(x)		(((x) + 0x0FFF) & ~0x0FFF)


/**************************************************************/


int verbose;

unsigned int memSize;
unsigned char *memory;

int loAddrSet;
unsigned int loAddr;
unsigned int hiAddr;


/**************************************************************/


void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  printf("error: ");
  vprintf(fmt, ap);
  printf("\n");
  va_end(ap);
  exit(1);
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
 * combined global symbol table
 */


#define INITIAL_NUM_BUCKETS	100


typedef struct symbol {
  /* symbol data */
  char *name;			/* the symbol's name */
  struct linkUnit *unit;	/* the symbol is defined there */
  unsigned int val;		/* the symbol's value */
  unsigned int attr;		/* the symbol's attributes */
  /* internal data */
  unsigned int hash;		/* full hash over symbol's name */
  struct symbol *next;		/* hash bucket chain */
} Symbol;


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
  p->unit = NULL;
  p->val = 0;
  p->attr = SYM_ATTR_U;
  p->hash = h;
  p->next = buckets[n];
  buckets[n] = p;
  numEntries++;
  return p;
}


int numberOfSymbols(void) {
  return numEntries;
}


/**************************************************************/


typedef struct linkUnit {
  char *path;			/* path to file of link unit */
  FILE *file;			/* file pointer to link unit */
  char *strs;			/* string space of link unit */
  char *libNames;		/* names of needed libraries */
  int libCount;			/* number of needed libraries */
  unsigned int offs;		/* load offset to link address */
  unsigned int addr;		/* virtual address of load point */
  unsigned int size;		/* address space used by link unit */
  struct symbol **syms;		/* array of pointers to symbols */
  int nsyms;			/* number of pointers to symbols */
  unsigned int orels;		/* offset of relocations in file */
  int nrels;			/* number of relocations in file */
  struct linkUnit *next;	/* next link unit, oder is important */
} LinkUnit;


LinkUnit *firstLinkUnit = NULL;
LinkUnit *lastLinkUnit;


LinkUnit *newLinkUnit(char *path) {
  LinkUnit *linkUnit;

  linkUnit = memAlloc(sizeof(LinkUnit));
  linkUnit->path = path;
  linkUnit->file = NULL;
  linkUnit->strs = NULL;
  linkUnit->libNames = NULL;
  linkUnit->libCount = 0;
  linkUnit->offs = 0;
  linkUnit->addr = 0;
  linkUnit->size = 0;
  linkUnit->syms = NULL;
  linkUnit->nsyms = 0;
  linkUnit->orels = 0;
  linkUnit->nrels = 0;
  linkUnit->next = NULL;
  if (firstLinkUnit == NULL) {
    firstLinkUnit = linkUnit;
  } else {
    lastLinkUnit->next = linkUnit;
  }
  lastLinkUnit = linkUnit;
  return linkUnit;
}


int isAlreadyLoaded(char *library) {
  LinkUnit *linkUnit;

  linkUnit = firstLinkUnit;
  while (linkUnit != NULL) {
    if (strcmp(library, linkUnit->path) == 0) {
      /* found */
      return 1;
    }
    linkUnit = linkUnit->next;
  }
  /* not found */
  return 0;
}


/**************************************************************/


void readObjHeader(EofHeader *hdr,
                   FILE *inFile, char *inPath,
                   unsigned int magic) {
  if (fseek(inFile, 0, SEEK_SET) < 0) {
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
  conv4FromEcoToNative((unsigned char *) &hdr->olibs);
  conv4FromEcoToNative((unsigned char *) &hdr->nlibs);
  if (hdr->magic != magic) {
    error("wrong magic number in input file '%s'", inPath);
  }
}


char *readObjStrings(unsigned int ostrs, unsigned int sstrs,
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


void readObjSegment(SegmentRecord *segRec,
                    unsigned int osegs, int segno,
                    FILE *inFile, char *inPath) {
  if (fseek(inFile, osegs + segno * sizeof(SegmentRecord), SEEK_SET) < 0) {
    error("cannot seek to segment record %d in input file '%s'",
          segno, inPath);
  }
  if (fread(segRec, sizeof(SegmentRecord), 1, inFile) != 1) {
    error("cannot read segment record %d in input file '%s'",
          segno, inPath);
  }
  conv4FromEcoToNative((unsigned char *) &segRec->name);
  conv4FromEcoToNative((unsigned char *) &segRec->offs);
  conv4FromEcoToNative((unsigned char *) &segRec->addr);
  conv4FromEcoToNative((unsigned char *) &segRec->size);
  conv4FromEcoToNative((unsigned char *) &segRec->attr);
}


void showObjSegment(SegmentRecord *segRec, char *strs) {
  printf("    loading segment '%s'\n", strs + segRec->name);
  printf("        offset  : 0x%08X\n", segRec->offs);
  printf("        address : 0x%08X\n", segRec->addr);
  printf("        size    : 0x%08X\n", segRec->size);
  printf("        attr    : ");
  if (segRec->attr & SEG_ATTR_A) {
    printf("A");
  } else {
    printf("-");
  }
  if (segRec->attr & SEG_ATTR_P) {
    printf("P");
  } else {
    printf("-");
  }
  if (segRec->attr & SEG_ATTR_W) {
    printf("W");
  } else {
    printf("-");
  }
  if (segRec->attr & SEG_ATTR_X) {
    printf("X");
  } else {
    printf("-");
  }
  printf("\n");
}


void readObjSymbol(SymbolRecord *symRec,
                   unsigned int osyms, int symno,
                   FILE *inFile, char *inPath) {
  if (fseek(inFile, osyms + symno * sizeof(SymbolRecord), SEEK_SET) < 0) {
    error("cannot seek to symbol record %d in input file '%s'",
          symno, inPath);
  }
  if (fread(symRec, sizeof(SymbolRecord), 1, inFile) != 1) {
    error("cannot read symbol record %d in input file '%s'",
          symno, inPath);
  }
  conv4FromEcoToNative((unsigned char *) &symRec->name);
  conv4FromEcoToNative((unsigned char *) &symRec->val);
  conv4FromEcoToNative((unsigned char *) &symRec->seg);
  conv4FromEcoToNative((unsigned char *) &symRec->attr);
}


void showObjSymbol(SymbolRecord *symRec, char *strs) {
  printf("    reading symbol '%s'\n", strs + symRec->name);
  printf("        value   : 0x%08X\n", symRec->val);
  printf("        segment : %d\n", symRec->seg);
  printf("        attr    : %c\n",
         (symRec->attr & SYM_ATTR_U) != 0 ? 'U' : 'D');
}


void readObjReloc(RelocRecord *relRec,
                  unsigned int orels, int relno,
                  FILE *inFile, char *inPath) {
  if (fseek(inFile, orels + relno * sizeof(RelocRecord), SEEK_SET) < 0) {
    error("cannot seek to relocation record %d in input file '%s'",
          relno, inPath);
  }
  if (fread(relRec, sizeof(RelocRecord), 1, inFile) != 1) {
    error("cannot read relocation record %d in input file '%s'",
          relno, inPath);
  }
  conv4FromEcoToNative((unsigned char *) &relRec->loc);
  conv4FromEcoToNative((unsigned char *) &relRec->seg);
  conv4FromEcoToNative((unsigned char *) &relRec->typ);
  conv4FromEcoToNative((unsigned char *) &relRec->ref);
  conv4FromEcoToNative((unsigned char *) &relRec->add);
}


char *relocMethod[] = {
  /*  0 */  "H16",
  /*  1 */  "L16",
  /*  2 */  "R16",
  /*  3 */  "R26",
  /*  4 */  "W32",
  /*  5 */  "GA_H16",
  /*  6 */  "GA_L16",
  /*  7 */  "GR_H16",
  /*  8 */  "GR_L16",
  /*  9 */  "GP_L16",
  /* 10 */  "ER_W32",
};


void showObjReloc(RelocRecord *relRec, int rn) {
  printf("    applying relocation %u\n", rn);
  printf("        loc     : 0x%08X\n", relRec->loc);
  printf("        seg     : ");
  if (relRec->seg == -1) {
    printf("*none*");
  } else {
    printf("%d", relRec->seg);
  }
  printf("\n");
  printf("        typ     : %s\n",
         relocMethod[relRec->typ & ~RELOC_SYM]);
  printf("        ref     : ");
  if (relRec->ref == -1) {
    printf("*none*");
  } else {
    printf("%s # %d",
           relRec->typ & RELOC_SYM ? "symbol" : "segment", relRec->ref);
  }
  printf("\n");
  printf("        add     : 0x%08X\n", relRec->add);
}


/**************************************************************/


LinkUnit *loadLinkUnit(char *inPath,
                       unsigned int magic,
                       unsigned int loadOff) {
  LinkUnit *linkUnit;
  FILE *inFile;
  EofHeader hdr;
  char *strs;
  int i;
  SegmentRecord segRec;
  unsigned char *dp;
  SymbolRecord symRec;
  Symbol *sym;

  /* allocate a new link unit structure */
  linkUnit = newLinkUnit(inPath);
  /* open input file */
  inFile = fopen(inPath, "r");
  if (inFile == NULL) {
    error("cannot open input file '%s'", inPath);
  }
  linkUnit->file = inFile;
  /* read header */
  readObjHeader(&hdr, inFile, inPath, magic);
  /* read strings */
  strs = readObjStrings(hdr.ostrs, hdr.sstrs, inFile, inPath);
  linkUnit->strs = strs;
  linkUnit->libNames = strs + hdr.olibs;
  linkUnit->libCount = hdr.nlibs;
  /* load segments */
  for (i = 0; i < hdr.nsegs; i++) {
    readObjSegment(&segRec, hdr.osegs, i, inFile, inPath);
    if (verbose) {
      showObjSegment(&segRec, strs);
    }
    if (segRec.attr & SEG_ATTR_A) {
      /* segment needs memory */
      if (!loAddrSet) {
        loAddrSet = 1;
        loAddr = segRec.addr + loadOff;
      }
      hiAddr = segRec.addr + loadOff + segRec.size;
      dp = memory + (segRec.addr + loadOff - loAddr);
      if (segRec.attr & SEG_ATTR_P) {
        /* load from file */
        if (fseek(inFile, hdr.odata + segRec.offs, SEEK_SET) < 0) {
          error("cannot seek to data of segment '%s' in file '%s'",
                strs + segRec.name, inPath);
        }
        if (fread(dp, 1, segRec.size, inFile) != segRec.size) {
          error("cannot read data of segment '%s' in file '%s'",
                strs + segRec.name, inPath);
        }
      } else {
        /* load with zeros */
        memset(dp, 0, segRec.size);
      }
    }
  }
  /* read symbols */
  linkUnit->nsyms = hdr.nsyms;
  linkUnit->syms = memAlloc(hdr.nsyms * sizeof(Symbol *));
  for (i = 0; i < hdr.nsyms; i++) {
    readObjSymbol(&symRec, hdr.osyms, i, inFile, inPath);
    if (verbose) {
      showObjSymbol(&symRec, strs);
    }
    sym = enterSymbol(strs + symRec.name);
    if ((symRec.attr & SYM_ATTR_U) == 0) {
      /* symbol gets defined here */
      if ((sym->attr & SYM_ATTR_U) == 0) {
        /* but is already defined in table */
        /* ignore multiple definitions */
      } else {
        /* copy symbol information to table */
        sym->unit = linkUnit;
        switch (symRec.seg) {
          case -1:
            /* absolute */
            sym->val = symRec.val;
            break;
          case 0:
            /* link-unit relative */
            sym->val = symRec.val + loadOff;
            break;
          default:
            /* illegal */
            sym->val = 0;
            error("illegal segment %d for symbol '%s'",
                  symRec.seg, sym->name);
            break;
        }
        sym->attr = symRec.attr;
      }
    }
    /* remember a pointer to the symbol in the link unit's symbol vector */
    /* so that relocations can easily access the symbol by its index */
    linkUnit->syms[i] = sym;
  }
  /* remember offset and number of relocations in file */
  linkUnit->orels = hdr.orels;
  linkUnit->nrels = hdr.nrels;
  return linkUnit;
}


void relocateLinkUnit(LinkUnit *linkUnit) {
  int i;
  RelocRecord relRec;
  unsigned int loadOff;
  unsigned char *dp;
  unsigned int word;
  Symbol *sym;

  /* apply relocations */
  for (i = 0; i < linkUnit->nrels; i++) {
    readObjReloc(&relRec, linkUnit->orels, i,
                 linkUnit->file, linkUnit->path);
    if (verbose) {
      showObjReloc(&relRec, i);
    }
    switch (relRec.typ & ~RELOC_SYM) {
      case RELOC_ER_W32:
        loadOff = linkUnit->offs;
        dp = memory + (relRec.loc + loadOff - loAddr);
        word = read4FromEco(dp);
        word += loadOff;
        write4ToEco(dp, word);
        break;
      case RELOC_W32:
        loadOff = linkUnit->offs;
        dp = memory + (relRec.loc + loadOff - loAddr);
        sym = linkUnit->syms[relRec.ref];
        if ((sym->attr & SYM_ATTR_U) != 0) {
          error("symbol '%s' not found (used in '%s')",
                sym->name, linkUnit->path);
        }
        word = sym->val;
        write4ToEco(dp, word);
        break;
      default:
        error("illegal relocation %d", relRec.typ & ~RELOC_SYM);
        break;
    }
  }
  /* close input file */
  fclose(linkUnit->file);
  linkUnit->file = NULL;
}


/**************************************************************/


/*
 * This function loads an executable together with all dynamic
 * libraries needed. The libraries are loaded in breadth-first
 * order. Variable 'currUnit' points to the link unit which is
 * currently scanned for libraries which need to be loaded.
 * Variable 'lastUnit' points to the link unit just loaded
 * (i.e., the current end of the work list).
 *
 * While the link units are loaded, a combined global symbol
 * table is built.
 *
 * After all link units have been loaded, the relocations of
 * all of them are applied.
 */
void loadExecutable(char *execPath, char *libPath, unsigned int loadOff) {
  LinkUnit *execUnit;
  LinkUnit *currUnit;
  LinkUnit *lastUnit;
  int pathSize;
  char *libName;
  int nameSize;
  char *library;
  int i;

  /* load executable */
  if (verbose) {
    printf("loading from executable '%s'\n", execPath);
    if (loadOff != 0) {
      printf("(with load offset 0x%08X)\n", loadOff);
    }
  }
  execUnit = loadLinkUnit(execPath, EOF_X_MAGIC, loadOff);
  execUnit->offs = loadOff;
  execUnit->addr = loAddr;
  execUnit->size = hiAddr - loAddr;
  /* load shared objects */
  pathSize = strlen(libPath);
  currUnit = execUnit;
  do {
    libName = currUnit->libNames;
    for (i = 0; i < currUnit->libCount; i++) {
      nameSize = strlen(libName);
      library = memAlloc(pathSize + nameSize + 2);
      strcpy(library, libPath);
      if (library[pathSize - 1] != '/') {
        library[pathSize] = '/';
        strcpy(library + pathSize + 1, libName);
      } else {
        strcpy(library + pathSize, libName);
      }
      if (isAlreadyLoaded(library)) {
        /* this file has already been loaded, skip */
        if (verbose) {
          printf("skipping file '%s', already loaded\n", library);
        }
      } else {
        /* this file has not been loaded yet */
        if (verbose) {
          printf("loading from shared object '%s'\n", library);
        }
        loadOff = PAGE_ALIGN(hiAddr);
        lastUnit = loadLinkUnit(library, EOF_D_MAGIC, loadOff);
        lastUnit->offs = loadOff;
        lastUnit->addr = loadOff;
        lastUnit->size = hiAddr - loadOff;
      }
      /* advance to next library name */
      while (*libName++ != '\0') ;
    }
    currUnit = currUnit->next;
  } while (currUnit != NULL);
  /* apply relocations */
  currUnit = execUnit;
  do {
    if (verbose) {
      printf("relocating link unit '%s'\n", currUnit->path);
    }
    relocateLinkUnit(currUnit);
    currUnit = currUnit->next;
  } while (currUnit != NULL) ;
}


/**************************************************************/


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


void showCombinedGST(void) {
  LinkUnit *linkUnit;
  int i;
  Symbol **tbl;
  int numSymbols;

  tbl = memAlloc(numberOfSymbols() * sizeof(Symbol *));
  printf("combined global symbol table\n");
  linkUnit = firstLinkUnit;
  while (linkUnit != NULL) {
    printf("    link unit : %s\n", linkUnit->path);
    printf("    load offs : 0x%08X\n", linkUnit->offs);
    printf("    load addr : 0x%08X\n", linkUnit->addr);
    printf("    byte size : 0x%08X\n", linkUnit->size);
    numSymbols = 0;
    for (i = 0; i < linkUnit->nsyms; i++) {
      if (linkUnit->syms[i]->unit == linkUnit) {
        tbl[numSymbols++] = linkUnit->syms[i];
      }
    }
    qsort(tbl, numSymbols, sizeof(Symbol *), compareSymbols);
    for (i = 0; i < numSymbols; i++) {
      printf("        %-24s  0x%08X\n",
             tbl[i]->name, tbl[i]->val);
    }
    linkUnit = linkUnit->next;
  }
  memFree(tbl);
}


/**************************************************************/


void writeBinary(char *binaryPath) {
  FILE *binaryFile;
  unsigned int size;

  if (verbose) {
    printf("writing to binary file '%s'\n", binaryPath);
  }
  binaryFile = fopen(binaryPath, "w");
  if (binaryFile == NULL) {
    error("cannot open binary file '%s'", binaryPath);
  }
  if (loAddrSet != 0 && loAddr < hiAddr) {
    size = hiAddr - loAddr;
    if (fwrite(memory, 1, size, binaryFile) != size) {
      error("cannot write to binary file '%s'", binaryPath);
    }
  }
  fclose(binaryFile);
}


/**************************************************************/


void usage(char *myself) {
  printf("usage: %s [options] <executable> <binary>\n", myself);
  printf("valid options are:\n");
  printf("    -v             verbose\n");
  printf("    -d <path>      search for libraries in <path>\n");
  printf("    -l <n>         load with n bytes offset\n");
  printf("    -m <n>         set maximum memory size to <n> MB\n");
  exit(1);
}


int main(int argc, char *argv[]) {
  char *libPath;
  unsigned int loadOff;
  unsigned int msize;
  char *execFileName;
  char *binFileName;
  int i;
  char *argp;
  char *endp;

  verbose = 0;
  libPath = "./";
  loadOff = 0;
  msize = DEFAULT_MEMSIZE;
  execFileName = NULL;
  binFileName = NULL;
  for (i = 1; i < argc; i++) {
    argp = argv[i];
    if (*argp == '-') {
      /* option */
      if (strcmp(argp, "-v") == 0) {
        verbose = 1;
      } else
      if (strcmp(argp, "-d") == 0) {
        if (i == argc - 1) {
          usage(argv[0]);
        }
        libPath = argv[++i];
      } else
      if (strcmp(argp, "-l") == 0) {
        if (i == argc - 1) {
          usage(argv[0]);
        }
        loadOff = strtoul(argv[++i], &endp, 0);
        if (*endp != '\0') {
          error("illegal load offset");
        }
      } else
      if (strcmp(argp, "-m") == 0) {
        if (i == argc - 1) {
          usage(argv[0]);
        }
        msize = strtoul(argv[++i], &endp, 0);
        if (*endp != '\0' ||
            msize <= 0 ||
            msize > MAX_MEMSIZE) {
          error("illegal memory size");
        }
      } else {
        usage(argv[0]);
      }
    } else {
      /* file */
      if (execFileName == NULL) {
        execFileName = argp;
      } else
      if (binFileName == NULL) {
        binFileName = argp;
      } else{
        usage(argv[0]);
      }
    }
  }
  if (execFileName == NULL) {
    error("no executable file specified");
  }
  if (binFileName == NULL) {
    error("no binary file specified");
  }
  if (verbose) {
    printf("initializing %u MB of memory\n", msize);
  }
  memSize = msize << 20;
  memory = memAlloc(memSize);
  for (i = 0; i < memSize; i++) {
    memory[i] = rand();
  }
  loAddrSet = 0;
  loadExecutable(execFileName, libPath, loadOff);
  if (verbose) {
    showCombinedGST();
  }
  writeBinary(binFileName);
  return 0;
}
