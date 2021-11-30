/*
 * ld.c -- ECO32 link editor
 */


/*
 * This is the solution to exercise 3:
 * Read a list of object modules and allocate memory for them.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "eof.h"


#define DEFAULT_CODE_BASE	0x1000
#define DEFAULT_OUT_NAME	"a.out"

#define WORD_ALIGN(x)		(((x) + 0x03) & ~0x03)
#define PAGE_ALIGN(x)		(((x) + 0x0FFF) & ~0x0FFF)


/**************************************************************/


int debugSegments = 1;
int debugGroups = 1;


/**************************************************************/


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
  char *name;			/* module name */
  unsigned char *data;		/* data space */
  char *strs;			/* string space */
  int nsegs;			/* number of segments */
  struct segment *segs;		/* array of segments */
  int nsyms;			/* number of symbols */
  struct symbol *syms;		/* array of symbols */
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


typedef struct segment {
  char *name;			/* segment name */
  unsigned char *data;		/* segment data */
  unsigned int addr;		/* virtual start address */
  unsigned int size;		/* size of segment in bytes */
  unsigned int npad;		/* number of padding bytes */
  unsigned int attr;		/* segment attributes */
} Segment;


typedef struct symbol {
  char *name;			/* symbol name */
  int val;			/* the symbol's value */
  int seg;			/* the symbol's segment, -1: absolute */
  unsigned int attr;		/* symbol attributes */
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


void readObjHeader(ExecHeader *hdr, FILE *inFile, char *inPath) {
  if (fseek(inFile, 0, SEEK_SET) < 0) {
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
  Symbol *sym;
  SymbolRecord symRec;

  mod->nsyms = nsyms;
  mod->syms = memAlloc(nsyms * sizeof(Symbol));
  if (fseek(inFile, osyms, SEEK_SET) < 0) {
    error("cannot seek to symbol table in input file '%s'", inPath);
  }
  for (i = 0; i < nsyms; i++) {
    sym = mod->syms + i;
    if (fread(&symRec, sizeof(SymbolRecord), 1, inFile) != 1) {
      error("cannot read symbol record in input file '%s'", inPath);
    }
    conv4FromEcoToNative((unsigned char *) &symRec.name);
    conv4FromEcoToNative((unsigned char *) &symRec.val);
    conv4FromEcoToNative((unsigned char *) &symRec.seg);
    conv4FromEcoToNative((unsigned char *) &symRec.attr);
    sym->name = mod->strs + symRec.name;
    sym->val = symRec.val;
    sym->seg = symRec.seg;
    sym->attr = symRec.attr;
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


Module *readObjModule(char *name, FILE *inFile, char *inPath) {
  ExecHeader hdr;
  Module *mod;

  mod = newModule(name);
  readObjHeader(&hdr, inFile, inPath);
  readObjData(mod, hdr.odata, hdr.sdata, inFile, inPath);
  readObjStrings(mod, hdr.ostrs, hdr.sstrs, inFile, inPath);
  readObjSegments(mod, hdr.osegs, hdr.nsegs, inFile, inPath);
  readObjSymbols(mod, hdr.osyms, hdr.nsyms, inFile, inPath);
  readObjRelocs(mod, hdr.orels, hdr.nrels, inFile, inPath);
  return mod;
}


/**************************************************************/


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
    if (magic == EXEC_MAGIC) {
      baseName = file->path + strlen(file->path);
      while (baseName != file->path && *baseName != '/') {
        baseName--;
      }
      if (*baseName == '/') {
        baseName++;
      }
      readObjModule(baseName, inFile, file->path);
    } else {
      error("input file '%s' is not an object file", file->path);
    }
    fclose(inFile);
    file = file->next;
  }
}


/**************************************************************/


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


void allocateStorage(unsigned int codeBase, int dataPageAlign) {
  Module *mod;
  Segment *seg;
  int i;
  unsigned int addr;
  unsigned int size;

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
}


/**************************************************************/


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


static ExecHeader execHeader;
static unsigned int outFileOffset;


static void writeDummyHeader(FILE *outFile) {
  memset(&execHeader, 0, sizeof(ExecHeader));
  if (fwrite(&execHeader, sizeof(ExecHeader), 1, outFile) != 1) {
    error("cannot write output file header");
  }
  /* init file offset */
  outFileOffset = sizeof(ExecHeader);
}


static void writeRealHeader(unsigned int entry, FILE *outFile) {
  execHeader.magic = EXEC_MAGIC;
  execHeader.entry = entry;
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
  if (fseek(outFile, 0, SEEK_SET) < 0) {
    error("cannot seek to header in output file");
  }
  if (fwrite(&execHeader, sizeof(ExecHeader), 1, outFile) != 1) {
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
      execHeader.sdata += size + npad;
    }
    part = part->next;
  }
}


void writeDataForTotals(TotalSegment *total, FILE *outFile) {
  while (total != NULL) {
    total->dataOffs = execHeader.sdata;
    writeDataForParts(total->firstPart, outFile);
    total = total->next;
  }
}


void writeDataForGroups(FILE *outFile) {
  execHeader.odata = outFileOffset;
  execHeader.sdata = 0;
  writeDataForTotals(groupAPX.firstTotal, outFile);
  writeDataForTotals(groupAPW.firstTotal, outFile);
  /* update file offset */
  outFileOffset += execHeader.sdata;
}


void writeStringsForTotals(TotalSegment *total, FILE *outFile) {
  unsigned int size;

  while (total != NULL) {
    total->nameOffs = execHeader.sstrs;
    size = strlen(total->name) + 1;
    if (fwrite(total->name, 1, size, outFile) != size) {
      error("cannot write output file strings");
    }
    execHeader.sstrs += size;
    total = total->next;
  }
}


void writeStringsForGroups(FILE *outFile) {
  execHeader.ostrs = outFileOffset;
  execHeader.sstrs = 0;
  writeStringsForTotals(groupAPX.firstTotal, outFile);
  writeStringsForTotals(groupAPW.firstTotal, outFile);
  writeStringsForTotals(groupAW.firstTotal, outFile);
  /* update file offset */
  outFileOffset += execHeader.sstrs;
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
    execHeader.nsegs++;
    total = total->next;
  }
}


void writeSegmentsForGroups(FILE *outFile) {
  execHeader.osegs = outFileOffset;
  execHeader.nsegs = 0;
  writeSegmentForTotals(groupAPX.firstTotal, outFile);
  writeSegmentForTotals(groupAPW.firstTotal, outFile);
  writeSegmentForTotals(groupAW.firstTotal, outFile);
}


void writeObjModule(unsigned int entry, char *outPath) {
  FILE *outFile;

  outFile = fopen(outPath, "w");
  if (outFile == NULL) {
    error("cannot open output file '%s'", outPath);
  }
  writeDummyHeader(outFile);
  writeDataForGroups(outFile);
  writeStringsForGroups(outFile);
  writeSegmentsForGroups(outFile);
  writeRealHeader(entry, outFile);
  fclose(outFile);
}


/**************************************************************/


void usage(char *myself) {
  fprintf(stderr, "usage: %s\n", myself);
  fprintf(stderr, "         [-d]             data directly after code\n");
  fprintf(stderr, "         [-c <addr>]      set code base address\n");
  fprintf(stderr, "         [-o <outfile>]   set output file name\n");
  fprintf(stderr, "         <infile> ...     input file names\n");
  exit(1);
}


int main(int argc, char *argv[]) {
  int i;
  int dataPageAlign;
  unsigned int codeBase;
  char *outName;
  char *endptr;

  dataPageAlign = 1;
  codeBase = DEFAULT_CODE_BASE;
  outName = DEFAULT_OUT_NAME;
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
      if (strcmp(argv[i], "-o") == 0) {
        if (++i == argc) {
          error("option '-o' is missing a file name");
        }
        outName = argv[i];
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
    fprintf(stderr, "[segments in modules before storage allocation]\n");
    showSegments();
  }
  allocateStorage(codeBase, dataPageAlign);
  if (debugGroups) {
    fprintf(stderr, "[segments in groups after storage allocation]\n");
    showGroups();
  }
  writeObjModule(codeBase, outName);
  return 0;
}
