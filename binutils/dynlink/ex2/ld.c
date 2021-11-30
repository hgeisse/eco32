/*
 * ld.c -- ECO32 link editor
 */


/*
 * This is the solution to exercise 2:
 * Read and write a single object file.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "eof.h"


#define DEFAULT_OUT_NAME	"a.out"


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
} Module;


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
  return mod;
}


typedef struct segment {
  char *name;			/* segment name */
  unsigned char *data;		/* segment data */
  unsigned int addr;		/* virtual start address */
  unsigned int size;		/* size of segment in bytes */
  unsigned int attr;		/* segment attributes */
  /* used for output only */
  unsigned int nameOffs;	/* offset in string space */
  unsigned int dataOffs;	/* offset in segment data */
} Segment;


typedef struct symbol {
  char *name;			/* symbol name */
  int val;			/* the symbol's value */
  int seg;			/* the symbol's segment, -1: absolute */
  unsigned int attr;		/* symbol attributes */
  /* used for output only */
  unsigned int nameOffs;	/* offset in string space */
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


Module *readObjModule(char *inPath) {
  FILE *inFile;
  ExecHeader hdr;
  Module *mod;

  inFile = fopen(inPath, "r");
  if (inFile == NULL) {
    error("cannot open input file '%s'", inPath);
  }
  readObjHeader(&hdr, inFile, inPath);
  mod = newModule(inPath);
  readObjData(mod, hdr.odata, hdr.sdata, inFile, inPath);
  readObjStrings(mod, hdr.ostrs, hdr.sstrs, inFile, inPath);
  readObjSegments(mod, hdr.osegs, hdr.nsegs, inFile, inPath);
  readObjSymbols(mod, hdr.osyms, hdr.nsyms, inFile, inPath);
  readObjRelocs(mod, hdr.orels, hdr.nrels, inFile, inPath);
  fclose(inFile);
  return mod;
}


/**************************************************************/


static ExecHeader execHeader;
static unsigned int outFileOffset;


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
  if (fwrite(&execHeader, sizeof(ExecHeader), 1, outFile) != 1) {
    error("cannot write output file header");
  }
}


static void writeData(Module *mod, FILE *outFile) {
  int i;
  Segment *seg;
  unsigned int size;

  execHeader.odata = outFileOffset;
  execHeader.sdata = 0;
  for (i = 0; i < mod->nsegs; i++) {
    seg = mod->segs + i;
    seg->dataOffs = execHeader.sdata;
    if (seg->attr & SEG_ATTR_P) {
      size = seg->size;
      if (size != 0) {
        if (fwrite(seg->data, 1, size, outFile) != size) {
          error("cannot write output file segment data");
        }
      }
      execHeader.sdata += size;
    }
  }
  /* update file offset */
  outFileOffset += execHeader.sdata;
}


static void writeStrings(Module *mod, FILE *outFile) {
  int i;
  Segment *seg;
  Symbol *sym;
  unsigned int size;

  execHeader.ostrs = outFileOffset;
  execHeader.sstrs = 0;
  for (i = 0; i < mod->nsegs; i++) {
    seg = mod->segs + i;
    seg->nameOffs = execHeader.sstrs;
    size = strlen(seg->name) + 1;
    if (fwrite(seg->name, 1, size, outFile) != size) {
      error("cannot write output file strings");
    }
    execHeader.sstrs += size;
  }
  for (i = 0; i < mod->nsyms; i++) {
    sym = mod->syms + i;
    sym->nameOffs = execHeader.sstrs;
    size = strlen(sym->name) + 1;
    if (fwrite(sym->name, 1, size, outFile) != size) {
      error("cannot write output file strings");
    }
    execHeader.sstrs += size;
  }
  /* update file offset */
  outFileOffset += execHeader.sstrs;
}


static void writeSegments(Module *mod, FILE *outFile) {
  int i;
  Segment *seg;
  SegmentRecord segRec;

  execHeader.osegs = outFileOffset;
  execHeader.nsegs = mod->nsegs;
  for (i = 0; i < mod->nsegs; i++) {
    seg = mod->segs + i;
    segRec.name = seg->nameOffs;
    segRec.offs = seg->dataOffs;
    segRec.addr = seg->addr;
    segRec.size = seg->size;
    segRec.attr = seg->attr;
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
}


static void writeSymbols(Module *mod, FILE *outFile) {
  int i;
  Symbol *sym;
  SymbolRecord symRec;

  execHeader.osyms = outFileOffset;
  execHeader.nsyms = mod->nsyms;
  for (i = 0; i < mod->nsyms; i++) {
    sym = mod->syms + i;
    symRec.name = sym->nameOffs;
    symRec.val = sym->val;
    symRec.seg = sym->seg;
    symRec.attr = sym->attr;
    conv4FromNativeToEco((unsigned char *) &symRec.name);
    conv4FromNativeToEco((unsigned char *) &symRec.val);
    conv4FromNativeToEco((unsigned char *) &symRec.seg);
    conv4FromNativeToEco((unsigned char *) &symRec.attr);
    if (fwrite(&symRec, sizeof(SymbolRecord), 1, outFile) != 1) {
      error("cannot write output file symbol");
    }
    /* update file offset */
    outFileOffset += sizeof(SymbolRecord);
  }
}


static void writeRelocs(Module *mod, FILE *outFile) {
  int i;
  Reloc *rel;
  RelocRecord relRec;

  execHeader.orels = outFileOffset;
  execHeader.nrels = mod->nrels;
  for (i = 0; i < mod->nrels; i++) {
    rel = mod->rels + i;
    relRec.loc = rel->loc;
    relRec.seg = rel->seg;
    relRec.typ = rel->typ;
    relRec.ref = rel->ref;
    relRec.add = rel->add;
    conv4FromNativeToEco((unsigned char *) &relRec.loc);
    conv4FromNativeToEco((unsigned char *) &relRec.seg);
    conv4FromNativeToEco((unsigned char *) &relRec.typ);
    conv4FromNativeToEco((unsigned char *) &relRec.ref);
    conv4FromNativeToEco((unsigned char *) &relRec.add);
    if (fwrite(&relRec, sizeof(RelocRecord), 1, outFile) != 1) {
      error("cannot write output file relocation");
    }
    /* update file offset */
    outFileOffset += sizeof(RelocRecord);
  }
}


void writeObjModule(Module *mod, char *outPath) {
  FILE *outFile;

  outFile = fopen(outPath, "w");
  if (outFile == NULL) {
    error("cannot open output file '%s'", outPath);
  }
  outFileOffset = 0;
  writeDummyHeader(outFile);
  writeData(mod, outFile);
  writeStrings(mod, outFile);
  writeSegments(mod, outFile);
  writeSymbols(mod, outFile);
  writeRelocs(mod, outFile);
  writeRealHeader(outFile);
  fclose(outFile);
}


/**************************************************************/


void usage(char *myself) {
  printf("usage: %s [-o <outfile>] <infile>\n", myself);
  exit(1);
}


int main(int argc, char *argv[]) {
  int i;
  char *inName;
  char *outName;
  Module *mod;

  inName = NULL;
  outName = DEFAULT_OUT_NAME;
  for (i = 1; i < argc; i++) {
    if (*argv[i] == '-') {
      /* option */
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
      if (inName != NULL) {
        usage(argv[0]);
      }
      inName = argv[i];
    }
  }
  if (inName == NULL) {
    error("no input file");
  }
  mod = readObjModule(inName);
  writeObjModule(mod, outName);
  return 0;
}
