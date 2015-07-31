/*
 * vcdchk.c -- check the value of a signal in a VCD file
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>


#define MAX_LINE	200
#define MAX_NAME	1000


/**************************************************************/


typedef int Bool;

#define FALSE		0
#define TRUE		1


/**************************************************************/


Bool debugVCD = FALSE;
Bool debugCHK = FALSE;
Bool debugCheck = FALSE;


/**************************************************************/


void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  printf("Error: ");
  vprintf(fmt, ap);
  printf("\n");
  va_end(ap);
  exit(1);
}


void *allocate(int size) {
  void *res;

  res = malloc(size);
  if (res == NULL) {
    error("no memory");
  }
  return res;
}


void release(void *p) {
  if (p == NULL) {
    error("NULL pointer in release()");
  }
  free(p);
}


/**************************************************************/


#define KW_COMMENT	0
#define KW_DATE		1
#define KW_DUMPALL	2
#define KW_DUMPOFF	3
#define KW_DUMPON	4
#define KW_DUMPVARS	5
#define KW_END		6
#define KW_ENDDEFS	7
#define KW_SCOPE	8
#define KW_TIMESCALE	9
#define KW_UPSCOPE	10
#define KW_VAR		11
#define KW_VERSION	12


typedef struct {
  char *name;
  int number;
} Keyword;


static Keyword keywords[] = {
  { "comment",        KW_COMMENT   },
  { "date",           KW_DATE      },
  { "dumpall",        KW_DUMPALL   },
  { "dumpoff",        KW_DUMPOFF   },
  { "dumpon",         KW_DUMPON    },
  { "dumpvars",       KW_DUMPVARS  },
  { "end",            KW_END       },
  { "enddefinitions", KW_ENDDEFS   },
  { "scope",          KW_SCOPE     },
  { "timescale",      KW_TIMESCALE },
  { "upscope",        KW_UPSCOPE   },
  { "var",            KW_VAR       },
  { "version",        KW_VERSION   },
};


Keyword *lookupKeyword(char *name) {
  int lo, hi, tst;
  int res;

  lo = 0;
  hi = sizeof(keywords) / sizeof(keywords[0]) - 1;
  while (lo <= hi) {
    tst = (lo + hi) / 2;
    res = strcmp(keywords[tst].name, name);
    if (res == 0) {
      return &keywords[tst];
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


#define VT_EVENT	0
#define VT_INTEGER	1
#define VT_PARAMETER	2
#define VT_REAL		3
#define VT_REG		4
#define VT_SUPPLY0	5
#define VT_SUPPLY1	6
#define VT_TIME		7
#define VT_TRI		8
#define VT_TRI0		9
#define VT_TRI1		10
#define VT_TRIAND	11
#define VT_TRIOR	12
#define VT_TRIREG	13
#define VT_WAND		14
#define VT_WIRE		15
#define VT_WOR		16


typedef struct {
  char *name;
  int number;
} VarType;


static VarType varTypes[] = {
  { "event",     VT_EVENT     },
  { "integer",   VT_INTEGER   },
  { "parameter", VT_PARAMETER },
  { "real",      VT_REAL      },
  { "reg",       VT_REG       },
  { "supply0",   VT_SUPPLY0   },
  { "supply1",   VT_SUPPLY1   },
  { "time",      VT_TIME      },
  { "tri",       VT_TRI       },
  { "tri0",      VT_TRI0      },
  { "tri1",      VT_TRI1      },
  { "triand",    VT_TRIAND    },
  { "trior",     VT_TRIOR     },
  { "trireg",    VT_TRIREG    },
  { "wand",      VT_WAND      },
  { "wire",      VT_WIRE      },
  { "wor",       VT_WOR       },
};


VarType *lookupVarType(char *name) {
  int lo, hi, tst;
  int res;

  lo = 0;
  hi = sizeof(varTypes) / sizeof(varTypes[0]) - 1;
  while (lo <= hi) {
    tst = (lo + hi) / 2;
    res = strcmp(varTypes[tst].name, name);
    if (res == 0) {
      return &varTypes[tst];
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


typedef struct change {
  int code;			/* id code of variable */
  char *vectorValue;		/* vector value stored as string */
				/* NULL if value is scalar value */
  char scalarValue;		/* one of '0', '1', 'x', or 'z' */
				/* '\0' if value is vector value */
  struct change *next;		/* next value change in this step */
} Change;


Change *newChange(int code, char *vectorValue, char scalarValue) {
  Change *change;

  change = allocate(sizeof(Change));
  change->code = code;
  if (vectorValue != NULL) {
    change->vectorValue = allocate(strlen(vectorValue) + 1);
    strcpy(change->vectorValue, vectorValue);
    change->scalarValue = '\0';
  } else {
    change->vectorValue = NULL;
    change->scalarValue = scalarValue;
  }
  change->next = NULL;
  return change;
}


typedef struct step {
  int time;			/* simulation time for this step */
  struct step *prev;		/* previous time step */
  struct step *next;		/* next time step */
  Change *changes;		/* variables that changed in this step */
} Step;


Step *newStep(int time, Step *prev) {
  Step *step;

  step = allocate(sizeof(Step));
  step->time = time;
  step->prev = prev;
  step->next = NULL;
  step->changes = NULL;
  return step;
}


typedef struct var {
  int type;			/* one of VT_xxx */
  int size;			/* number of bits */
  int code;			/* id code of variable */
  char *name;			/* local name of variable */
  int msi;			/* most significant index */
  int lsi;			/* least significant index */
  struct var *next;		/* linked list of vars in scope */
} Var;


Var *newVar(int type, int size, int code,
            char *name, int msi, int lsi) {
  Var *var;

  var = allocate(sizeof(Var));
  var->type = type;
  var->size = size;
  var->code = code;
  var->name = allocate(strlen(name) + 1);
  strcpy(var->name, name);
  var->msi = msi;
  var->lsi = lsi;
  var->next = NULL;
  return var;
}


#define SCOPE_MODULE	0
#define SCOPE_TASK	1
#define SCOPE_FUNCTION	2
#define SCOPE_BEGIN	3
#define SCOPE_FORK	4

typedef struct scope {
  int number;			/* every scope has a unique number */
  int type;			/* one of SCOPE_xxx */
  char *name;			/* the name of the scope */
  struct scope *parent;
  struct scope *siblings;
  struct scope *children;
  struct scope *lastchild;
  Var *variables;
  Var *lastvar;
} Scope;


Scope *newScope(int type, char *name, Scope *parent) {
  static int scopeNumber = 0;
  Scope *scope;

  scope = allocate(sizeof(Scope));
  scope->number = scopeNumber++;
  scope->type = type;
  scope->name = allocate(strlen(name) + 1);
  strcpy(scope->name, name);
  scope->parent = parent;
  scope->siblings = NULL;
  scope->children = NULL;
  scope->lastchild = NULL;
  scope->variables = NULL;
  scope->lastvar = NULL;
  return scope;
}


#define VCD_TS_S	0
#define VCD_TS_MS	1
#define VCD_TS_US	2
#define VCD_TS_NS	3
#define VCD_TS_PS	4
#define VCD_TS_FS	5

typedef struct {
  char *date;			/* generation date */
  char *version;		/* simulator version */
  int timescaleMult;		/* 1, 10, 100 */
  int timescaleUnit;		/* one of VCD_TS_xx */
  Scope *root;			/* root scope */
  Step *steps;			/* time steps */
} VCD;


/**************************************************************/


#define INITIAL_HASH_SIZE	100


typedef struct entry {
  char *name;			/* key */
  Var *var;			/* value */
  unsigned hashValue;		/* hash value of name */
  struct entry *next;		/* bucket chaining */
} Entry;


static int hashSize = 0;
static Entry **buckets;
static int numEntries;


static unsigned hash(char *s) {
  unsigned h, g;

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


static Bool isPrime(int i) {
  int t;

  if (i < 2) {
    return FALSE;
  }
  if (i == 2) {
    return TRUE;
  }
  if (i % 2 == 0) {
    return FALSE;
  }
  t = 3;
  while (t * t <= i) {
    if (i % t == 0) {
      return FALSE;
    }
    t += 2;
  }
  return TRUE;
}


static void initTable(void) {
  int i;

  hashSize = INITIAL_HASH_SIZE;
  while (!isPrime(hashSize)) {
    hashSize++;
  }
  buckets = (Entry **) allocate(hashSize * sizeof(Entry *));
  for (i = 0; i < hashSize; i++) {
    buckets[i] = NULL;
  }
  numEntries = 0;
}


static void growTable(void) {
  int newHashSize;
  Entry **newBuckets;
  int i, n;
  Entry *p, *q;

  /* compute new hash size */
  newHashSize = 2 * hashSize + 1;
  while (!isPrime(newHashSize)) {
    newHashSize += 2;
  }
  /* init new hash table */
  newBuckets = (Entry **) allocate(newHashSize * sizeof(Entry *));
  for (i = 0; i < newHashSize; i++) {
    newBuckets[i] = NULL;
  }
  /* rehash old entries */
  for (i = 0; i < hashSize; i++) {
    p = buckets[i];
    while (p != NULL) {
      q = p;
      p = p->next;
      n = q->hashValue % newHashSize;
      q->next = newBuckets[n];
      newBuckets[n] = q;
    }
  }
  /* swap tables */
  release(buckets);
  buckets = newBuckets;
  hashSize = newHashSize;
}


Var *lookupVar(char *name) {
  unsigned hashValue;
  int n;
  Entry *p;

  /* check for no table at all */
  if (hashSize == 0) {
    /* not found */
    return NULL;
  }
  /* compute hash value and bucket number */
  hashValue = hash(name);
  n = hashValue % hashSize;
  /* search in bucket list */
  p = buckets[n];
  while (p != NULL) {
    if (p->hashValue == hashValue) {
      if (strcmp(p->name, name) == 0) {
        /* found: return variable */
        return p->var;
      }
    }
    p = p->next;
  }
  /* not found */
  return NULL;
}


void enterVar(char *name, Var *var) {
  char nameBuffer[500];
  unsigned hashValue;
  int n;
  Entry *p;

  /* concat module name and variable name */
  strcpy(nameBuffer, name);
  strcat(nameBuffer, var->name);
  /* initialize hash table if necessary */
  if (hashSize == 0) {
    initTable();
  }
  /* grow hash table if necessary */
  if (numEntries == hashSize) {
    growTable();
  }
  /* compute hash value and bucket number */
  hashValue = hash(nameBuffer);
  n = hashValue % hashSize;
  /* add new variable to bucket list */
  p = (Entry *) allocate(sizeof(Entry));
  p->name = (char *) allocate(strlen(nameBuffer) + 1);
  strcpy(p->name, nameBuffer);
  p->var = var;
  p->hashValue = hashValue;
  p->next = buckets[n];
  buckets[n] = p;
  numEntries++;
}


/**************************************************************/


static FILE *vcdFile;
static int vcdLine;


Bool skipBlanks(void) {
  int c;

  do {
    c = fgetc(vcdFile);
    if (c == EOF) {
      break;
    }
    if (c == '\n') {
      vcdLine++;
    }
  } while (c == ' ' || c == '\t' || c == '\n');
  if (c != EOF) {
    ungetc(c, vcdFile);
  }
  return c == EOF;
}


Keyword *getKeyword(void) {
  int c;
  char buf[MAX_LINE];
  char *p;

  c = fgetc(vcdFile);
  if (c == EOF) {
    return NULL;
  }
  if (c != '$') {
    ungetc(c, vcdFile);
    return NULL;
  }
  c = fgetc(vcdFile);
  if (c == EOF) {
    return NULL;
  }
  p = buf;
  while (isalpha(c)) {
    *p++ = c;
    c = fgetc(vcdFile);
  }
  ungetc(c, vcdFile);
  *p = '\0';
  return lookupKeyword(buf);
}


void skipPastEnd(void) {
  int c;
  Keyword *kwp;

  while (1) {
    do {
      c = fgetc(vcdFile);
      if (c == EOF) {
        error("unexpected end of VCD file");
      }
      if (c == '\n') {
        vcdLine++;
      }
    } while (c != '$');
    ungetc(c, vcdFile);
    kwp = getKeyword();
    if (kwp != NULL && kwp->number == KW_END) {
      return;
    }
  }
}


int collectTillDollar(char *buffer) {
  char *p;
  int c;

  p = buffer;
  do {
    c = fgetc(vcdFile);
    if (c == EOF) {
      error("unexpected end of VCD file");
    }
    if (c == '\n') {
      vcdLine++;
    }
    *p++ = c;
  } while (c != '$');
  ungetc(c, vcdFile);
  p -= 2;
  while (*p == ' ' || *p == '\t' || *p == '\n') {
    p--;
  }
  *++p = '\0';
  return p - buffer;
}


int getString(char *buffer) {
  char *p;
  int c;

  p = buffer;
  while (1) {
    c = fgetc(vcdFile);
    if (c == EOF) {
      error("unexpected end of VCD file");
    }
    if (c == ' ' || c == '\t' || c == '\n') {
      break;
    }
    *p++ = c;
  }
  ungetc(c, vcdFile);
  *p = '\0';
  return p - buffer;
}


Bool getIdent(char *buffer) {
  char *p;
  int c;

  c = fgetc(vcdFile);
  if (c == EOF) {
    return FALSE;
  }
  if (!isalpha(c) && c != '_') {
    ungetc(c, vcdFile);
    return FALSE;
  }
  p = buffer;
  while (isalnum(c) || c == '_' || c == '$') {
    *p++ = c;
    c = fgetc(vcdFile);
  }
  if (c != EOF) {
    ungetc(c, vcdFile);
  }
  *p = '\0';
  return TRUE;
}


int getVarCode(void) {
  int varCode;
  int weight;
  int c;
  int i;

  varCode = 0;
  weight = 1;
  for (i = 0; i <= 4; i++) {
    c = fgetc(vcdFile);
    if (c < '!' || c > '~') {
      break;
    }
    if (i == 4) {
      error("line %d: too many base-94 digits", vcdLine);
    }
    varCode += weight * (c - '!');
    weight *= 94;
  }
  if (i == 0) {
    error("line %d: variable code expected", vcdLine);
  }
  if (c != EOF) {
    ungetc(c, vcdFile);
  }
  return varCode;
}


int getInteger(void) {
  int val;
  int c;

  val = 0;
  skipBlanks();
  c = fgetc(vcdFile);
  if (!isdigit(c)) {
    error("line %d: number expected", vcdLine);
  }
  while (1) {
    if (!isdigit(c)) {
      break;
    }
    val = val * 10 + (c - '0');
    c = fgetc(vcdFile);
  }
  if (c != EOF) {
    ungetc(c, vcdFile);
  }
  return val;
}


void getIndexRange(int *msip, int *lsip) {
  int c;

  *msip = -1;
  *lsip = -1;
  skipBlanks();
  c = fgetc(vcdFile);
  if (c == EOF) {
    return;
  }
  if (c != '[') {
    ungetc(c, vcdFile);
    return;
  }
  *msip = getInteger();
  skipBlanks();
  c = fgetc(vcdFile);
  if (c != ':' && c != ']') {
    error("line %d: ':' or ']' expected", vcdLine);
  }
  if (c == ':') {
    skipBlanks();
    *lsip = getInteger();
    skipBlanks();
    c = fgetc(vcdFile);
  }
  if (c != ']') {
    error("line %d: ']' expected", vcdLine);
  }
}


int getTime(void) {
  int c;

  c = fgetc(vcdFile);
  if (c != '#') {
    ungetc(c, vcdFile);
    return -1;
  }
  return getInteger();
}


Change *getChange(void) {
  int c;
  int varCode;
  Change *change;
  char buffer[MAX_LINE];
  char *p;

  c = fgetc(vcdFile);
  if (c != '0' && c != '1' && c != 'x' && c != 'X' &&
      c != 'z' && c != 'Z' && c != 'b' && c != 'B') {
    if (c == 'r' || c == 'R') {
      error("line %d: real numbers are not supported, sorry", vcdLine);
    }
    if (c != EOF) {
      ungetc(c, vcdFile);
    }
    return NULL;
  }
  if (c == 'b' || c == 'B') {
    /* vector */
    p = buffer;
    do {
      c = fgetc(vcdFile);
      switch (c) {
        case '0':
        case '1':
        case 'x':
        case 'z':
          *p++ = c;
          break;
        case 'X':
        case 'Z':
          *p++ = tolower(c);
          break;
        default:
          break;
      }
    } while (c == '0' || c == '1' || c == 'x' ||
             c == 'X' || c == 'z' || c == 'Z');
    if (c != EOF) {
      ungetc(c, vcdFile);
    }
    *p = '\0';
    skipBlanks();
    varCode = getVarCode();
    change = newChange(varCode, buffer, '\0');
  } else {
    /* scalar */
    if (c == 'X') {
      c = 'x';
    } else
    if (c == 'Z') {
      c = 'z';
    }
    varCode = getVarCode();
    change = newChange(varCode, NULL, c);
  }
  return change;
}


VCD *parseVCD(FILE *input) {
  VCD *vcd;
  int section;
  Keyword *kwp;
  char buffer[MAX_LINE];
  int length;
  char *endptr;
  Scope *currentScope;
  Scope *scope;
  int scopeType;
  char currentPrefix[MAX_NAME];
  VarType *vtp;
  int varType;
  int varSize;
  int varCode;
  int msi;
  int lsi;
  Var *var;
  Step *currentStep;
  Step *step;
  int time;
  Change *currentChange;
  Change *change;

  /* allocate a VCD structure */
  vcd = allocate(sizeof(VCD));
  vcd->date = NULL;
  vcd->version = NULL;
  vcd->timescaleMult = -1;
  vcd->timescaleUnit = -1;
  vcd->root = NULL;
  vcd->steps = NULL;
  /* start parsing */
  vcdFile = input;
  vcdLine = 1;
  section = 1;
  /* read header and node information */
  currentScope = NULL;
  currentPrefix[0] = '\0';
  while (section == 1) {
    skipBlanks();
    kwp = getKeyword();
    if (kwp == NULL) {
      error("line %d: keyword expected", vcdLine);
    }
    switch (kwp->number) {
      case KW_COMMENT:
        /* ignore comment */
        skipPastEnd();
        break;
      case KW_DATE:
        /* record date as string */
        skipBlanks();
        length = collectTillDollar(buffer);
        vcd->date = allocate(length + 1);
        strcpy(vcd->date, buffer);
        skipPastEnd();
        break;
      case KW_VERSION:
        /* record version as string */
        skipBlanks();
        length = collectTillDollar(buffer);
        vcd->version = allocate(length + 1);
        strcpy(vcd->version, buffer);
        skipPastEnd();
        break;
      case KW_TIMESCALE:
        /* record timescale as multiplier and unit */
        skipBlanks();
        length = collectTillDollar(buffer);
        /* get multiplier */
        vcd->timescaleMult = strtol(buffer, &endptr, 10);
        if (vcd->timescaleMult != 1 &&
            vcd->timescaleMult != 10 &&
            vcd->timescaleMult != 100) {
          error("line %d: illegal timescale multiplier (%d)",
                vcdLine, vcd->timescaleMult);
        }
        /* multiplier and unit may be separated by whitespace */
        while (*endptr == ' ' || *endptr == '\t') {
          endptr++;
        }
        /* get unit */
        if (endptr[0] == 's' && endptr[1] == '\0') {
          vcd->timescaleUnit = VCD_TS_S;
        } else
        if (endptr[0] == 'm' && endptr[1] == 's' && endptr[2] == '\0') {
          vcd->timescaleUnit = VCD_TS_MS;
        } else
        if (endptr[0] == 'u' && endptr[1] == 's' && endptr[2] == '\0') {
          vcd->timescaleUnit = VCD_TS_US;
        } else
        if (endptr[0] == 'n' && endptr[1] == 's' && endptr[2] == '\0') {
          vcd->timescaleUnit = VCD_TS_NS;
        } else
        if (endptr[0] == 'p' && endptr[1] == 's' && endptr[2] == '\0') {
          vcd->timescaleUnit = VCD_TS_PS;
        } else
        if (endptr[0] == 'f' && endptr[1] == 's' && endptr[2] == '\0') {
          vcd->timescaleUnit = VCD_TS_FS;
        } else {
          error("line %d: illegal timescale unit (%s)",
                vcdLine, endptr);
        }
        skipPastEnd();
        break;
      case KW_SCOPE:
        /* open a new scope */
        skipBlanks();
        /* get type */
        getString(buffer);
        if (strcmp(buffer, "module") == 0) {
          scopeType = SCOPE_MODULE;
        } else
        if (strcmp(buffer, "task") == 0) {
          scopeType = SCOPE_TASK;
        } else
        if (strcmp(buffer, "function") == 0) {
          scopeType = SCOPE_FUNCTION;
        } else
        if (strcmp(buffer, "begin") == 0) {
          scopeType = SCOPE_BEGIN;
        } else
        if (strcmp(buffer, "fork") == 0) {
          scopeType = SCOPE_FORK;
        } else {
          error("line %d: unknown scope type '%s'", vcdLine, buffer);
        }
        skipBlanks();
        /* get name */
        if (!getIdent(buffer)) {
          error("line %d: identifier expected", vcdLine);
        }
        /* allocate and link a new scope */
        scope = newScope(scopeType, buffer, currentScope);
        if (currentScope == NULL) {
          /* new scope is a top-level scope */
          if (vcd->root == NULL) {
            /* first top-level scope */
            vcd->root = scope;
          } else {
            /* another top-level scope */
            /* append, use currentScope as auxiliary variable */
            currentScope = vcd->root;
            while (currentScope->siblings != NULL) {
              currentScope = currentScope->siblings;
            }
            currentScope->siblings = scope;
          }
        } else {
          /* new scope is not a top-level scope */
          if (currentScope->children == NULL) {
            currentScope->children = scope;
          } else {
            currentScope->lastchild->siblings = scope;
          }
          currentScope->lastchild = scope;
        }
        currentScope = scope;
        /* augment current prefix */
        strcat(currentPrefix, currentScope->name);
        strcat(currentPrefix, ".");
        skipPastEnd();
        break;
      case KW_UPSCOPE:
        /* close the current scope */
        if (currentScope == NULL) {
          error("line %d: no $scope for this $upscope", vcdLine);
        }
        currentScope = currentScope->parent;
        /* prune current prefix */
        length = strlen(currentPrefix) - 2;
        while (length >= 0 && currentPrefix[length] != '.') {
          length--;
        }
        length++;
        currentPrefix[length] = '\0';
        skipPastEnd();
        break;
      case KW_VAR:
        /* record a new variable */
        skipBlanks();
        /* get type */
        getString(buffer);
        vtp = lookupVarType(buffer);
        if (vtp == NULL) {
          error("line %d: unknown variable type '%s'", vcdLine, buffer);
        }
        varType = vtp->number;
        skipBlanks();
        /* get size */
        getString(buffer);
        varSize = strtol(buffer, &endptr, 10);
        if (*endptr != '\0' || varSize <= 0) {
          error("line %d: cannot read variable size", vcdLine);
        }
        skipBlanks();
        /* get code */
        varCode = getVarCode();
        skipBlanks();
        /* get name */
        if (!getIdent(buffer)) {
          error("line %d: identifier expected", vcdLine);
        }
        skipBlanks();
        /* get optional index range */
        getIndexRange(&msi, &lsi);
        /* allocate and link a new variable */
        var = newVar(varType, varSize, varCode, buffer, msi, lsi);
        if (currentScope->variables == NULL) {
          currentScope->variables = var;
        } else {
          currentScope->lastvar->next = var;
        }
        currentScope->lastvar = var;
        /* additionally, enter variable in var table */
        enterVar(currentPrefix, var);
        skipPastEnd();
        break;
      case KW_ENDDEFS:
        /* end the first section of the VCD file */
        section = 2;
        skipPastEnd();
        break;
      default:
        error("line %d: unexpected keyword '$%s'", vcdLine, kwp->name);
        break;
    }
  }
  /* read value changes */
  currentStep = NULL;
  currentChange = NULL;
  while (1) {
    if (skipBlanks()) {
      break;
    }
    kwp = getKeyword();
    if (kwp != NULL) {
      switch (kwp->number) {
        case KW_COMMENT:
          /* ignore comment */
          skipPastEnd();
          break;
        case KW_DUMPVARS:
        case KW_DUMPALL:
        case KW_DUMPOFF:
        case KW_DUMPON:
          /* record value changes */
          while (1) {
            skipBlanks();
            change = getChange();
            if (change == NULL) {
              break;
            }
            if (currentChange == NULL) {
              if (currentStep == NULL) {
                error("line %d: value change without simulation time",
                      vcdLine);
              }
              currentStep->changes = change;
            } else {
              currentChange->next = change;
            }
            currentChange = change;
          }
          skipPastEnd();
          break;
        default:
          error("line %d: unexpected keyword '%s'", vcdLine, kwp->name);
          break;
      }
      continue;
    }
    time = getTime();
    if (time >= 0) {
      if (currentStep != NULL && currentStep->time >= time) {
        error("line %d: time step has illegal simulation time", vcdLine);
      }
      step = newStep(time, currentStep);
      if (currentStep == NULL) {
        vcd->steps = step;
      } else {
        currentStep->next = step;
      }
      currentStep = step;
      currentChange = NULL;
      continue;
    }
    change = getChange();
    if (change != NULL) {
      if (currentChange == NULL) {
        if (currentStep == NULL) {
          error("line %d: value change without simulation time", vcdLine);
        }
        currentStep->changes = change;
      } else {
        currentChange->next = change;
      }
      currentChange = change;
      continue;
    }
    error("line %d: keyword, time, or value change expected", vcdLine);
  }
  /* done */
  return vcd;
}


char *varType[] = {
  "event",
  "integer",
  "parameter",
  "real",
  "reg",
  "supply0",
  "supply1",
  "time",
  "tri",
  "tri0",
  "tri1",
  "triand",
  "trior",
  "trireg",
  "wand",
  "wire",
  "wor",
};


char *scopeType[] = {
  "module",
  "task",
  "function",
  "begin",
  "fork",
};


void showVarPrefix(Scope *scope) {
  if (scope == NULL) {
    return;
  }
  showVarPrefix(scope->parent);
  printf("%s.", scope->name);
}


void showScope(Scope *scope) {
  Var *var;

  printf("    scope #%d\n", scope->number);
  printf("        type       :  %s\n", scopeType[scope->type]);
  printf("        name       :  %s\n", scope->name);
  printf("        parent     :  ");
  if (scope->parent == NULL) {
    printf("-- none --");
  } else {
    printf("scope #%d", scope->parent->number);
  }
  printf("\n");
  printf("        siblings   :  ");
  if (scope->siblings == NULL) {
    printf("-- none --");
  } else {
    printf("scope #%d", scope->siblings->number);
  }
  printf("\n");
  printf("        children   :  ");
  if (scope->children == NULL) {
    printf("-- none --");
  } else {
    printf("scope #%d", scope->children->number);
  }
  printf("\n");
  printf("        lastchild  :  ");
  if (scope->lastchild == NULL) {
    printf("-- none --");
  } else {
    printf("scope #%d", scope->lastchild->number);
  }
  printf("\n");
  printf("        var prefix :  ");
  showVarPrefix(scope);
  printf("\n");
  printf("        variables  :  ");
  if (scope->variables == NULL) {
    printf("-- none --\n");
  } else {
    printf("\n");
    var = scope->variables;
    while (var != NULL) {
      printf("            ");
      printf("%s", var->name);
      if (var->msi != -1) {
        printf(" [%d", var->msi);
        if (var->lsi != -1) {
          printf(":%d", var->lsi);
        }
        printf("]");
      }
      printf("\n");
      printf("                ");
      printf("type = %-9s   ", varType[var->type]);
      printf("size = %4d    ", var->size);
      printf("code = %6d\n", var->code);
      var = var->next;
    }
  }
  if (scope->siblings != NULL) {
    showScope(scope->siblings);
  }
  if (scope->children != NULL) {
    showScope(scope->children);
  }
}


void showSteps(Step *steps) {
  Change *changes;

  while (steps != NULL) {
    printf("    #%d\n", steps->time);
    changes = steps->changes;
    while (changes != NULL) {
      printf("        code %8d : value ", changes->code);
      if (changes->vectorValue != NULL) {
        printf("'%s'", changes->vectorValue);
      } else {
        printf("'%c'", changes->scalarValue);
      }
      printf("\n");
      changes = changes->next;
    }
    steps = steps->next;
  }
}


char *tsUnit[6] = {
  "s", "ms", "us",
  "ns", "ps", "fs"
};


void showVCD(VCD *vcd) {
  printf("Value Change Dump Data\n");

  printf("date          :  ");
  if (vcd->date == NULL) {
    printf("-- none --");
  } else {
    printf("%s", vcd->date);
  }
  printf("\n");

  printf("version       :  ");
  if (vcd->version == NULL) {
    printf("-- none --");
  } else {
    printf("%s", vcd->version);
  }
  printf("\n");

  printf("timescale     :  ");
  if (vcd->timescaleMult < 0 ||
      vcd->timescaleUnit < 0) {
    printf("-- none --");
  } else {
    printf("%d %s", vcd->timescaleMult, tsUnit[vcd->timescaleUnit]);
  }
  printf("\n");

  printf("root scope    :  ");
  if (vcd->root == NULL) {
    printf("-- none --");
    printf("\n");
  } else {
    printf("%d", vcd->root->number);
    printf("\n");
    showScope(vcd->root);
  }

  printf("value changes :  ");
  if (vcd->steps == NULL) {
    printf("-- none --");
    printf("\n");
  } else {
    printf("\n");
    showSteps(vcd->steps);
  }
}


/**************************************************************/


typedef struct chk {
  int timeMult;		/* this many timeUnits */
  int timeUnit;		/* one of VCD_TS_xxx */
  char *name;		/* name of variable to check */
  int width;		/* width of variable to check */
  char *value;		/* value to check variable against */
  struct chk *next;	/* list elements are connected by 'and' */
} CHK;


void shift(char *buffer, int width, int amount, char *val) {
  int j, i;

  for (j = 0; j < amount; j++) {
    for (i = 0; i < width - 1; i++) {
      buffer[i] = buffer[i + 1];
    }
    buffer[i] = val[j];
  }
}


char *bitval1[2] = {
  "0", "1"
};


char *bitval4[16] = {
  "0000", "0001", "0010", "0011",
  "0100", "0101", "0110", "0111",
  "1000", "1001", "1010", "1011",
  "1100", "1101", "1110", "1111"
};


int skipSpace(int c, FILE *chkFile) {
  while (1) {
    while (c == ' ' || c == '\t' || c == '\n') {
      c = fgetc(chkFile);
    }
    if (c != '/') {
      return c;
    }
    c = fgetc(chkFile);
    if (c == EOF) {
      error("chk spec: unexpected end of input");
    }
    if (c != '/') {
      ungetc(c, chkFile);
      return '/';
    }
    do {
      c = fgetc(chkFile);
      if (c == EOF) {
        return EOF;
      }
    } while (c != '\n');
  }
}


CHK *parseSingleChk(FILE *chkFile) {
  CHK *chk;
  int c, d;
  char name[MAX_LINE];
  char *p;
  int base;
  int i;

  chk = allocate(sizeof(CHK));
  c = fgetc(chkFile);
  c = skipSpace(c, chkFile);
  chk->timeMult = 0;
  if (!isdigit(c)) {
    error("chk spec: time multiplier does not start with a digit");
  }
  while (isdigit(c)) {
    chk->timeMult = chk->timeMult * 10 + (c - '0');
    c = fgetc(chkFile);
  }
  c = skipSpace(c, chkFile);
  if (c == 's') {
    chk->timeUnit = VCD_TS_S;
  } else {
    d = fgetc(chkFile);
    if (c == 'm' && d == 's') {
      chk->timeUnit = VCD_TS_MS;
    } else
    if (c == 'u' && d == 's') {
      chk->timeUnit = VCD_TS_US;
    } else
    if (c == 'n' && d == 's') {
      chk->timeUnit = VCD_TS_NS;
    } else
    if (c == 'p' && d == 's') {
      chk->timeUnit = VCD_TS_PS;
    } else
    if (c == 'f' && d == 's') {
      chk->timeUnit = VCD_TS_FS;
    } else {
      error("chk spec: illegal time unit");
    }
  }
  c = fgetc(chkFile);
  c = skipSpace(c, chkFile);
  p = name;
  if (!isalpha(c) && c != '_') {
    error("chk spec: name does not start with a letter");
  }
  while (isalnum(c) || c == '_' || c == '$' || c == '.') {
    *p++ = c;
    c = fgetc(chkFile);
  }
  *p = '\0';
  chk->name = allocate(p - name + 1);
  strcpy(chk->name, name);
  c = skipSpace(c, chkFile);
  chk->width = 0;
  if (!isdigit(c)) {
    error("chk spec: bit width does not start with a digit");
  }
  while (isdigit(c)) {
    chk->width = chk->width * 10 + (c - '0');
    c = fgetc(chkFile);
  }
  if (chk->width <= 0) {
    error("chk spec: illegal bit width");
  }
  chk->value = allocate(chk->width + 1);
  if (c != '\'') {
    error("chk spec: apostrophe missing after bit width");
  }
  c = fgetc(chkFile);
  if (c == 'b') {
    base = 2;
  } else
  if (c == 'h') {
    base = 16;
  } else {
    error("chk spec: illegal number base");
  }
  c = fgetc(chkFile);
  if (c == 'x' || c == 'X') {
    for (i = 0; i < chk->width; i++) {
      chk->value[i] = 'x';
    }
  } else
  if (c == 'z' || c == 'Z') {
    for (i = 0; i < chk->width; i++) {
      chk->value[i] = 'z';
    }
  } else {
    for (i = 0; i < chk->width; i++) {
      chk->value[i] = '0';
    }
  }
  chk->value[chk->width] = '\0';
  if (base == 2) {
    if (c != 'x' && c != 'X' &&
        c != 'z' && c != 'Z' &&
        c != '0' && c != '1') {
      error("chk spec: base-2 number does not start with binary digit");
    }
    while (1) {
      if (c == 'x' || c == 'X') {
        shift(chk->value, chk->width, 1, "x");
      } else
      if (c == 'z' || c == 'Z') {
        shift(chk->value, chk->width, 1, "z");
      } else
      if (c == '0' || c == '1') {
        shift(chk->value, chk->width, 1, bitval1[c - '0']);
      } else {
        break;
      }
      c = fgetc(chkFile);
    }
  } else
  if (base == 16) {
    if (c != 'x' && c != 'X' &&
        c != 'z' && c != 'Z' &&
        !isxdigit(c)) {
      error("chk spec: base-16 number does not start with hex digit");
    }
    while (1) {
      if (c == 'x' || c == 'X') {
        shift(chk->value, chk->width, 4, "xxxx");
      } else
      if (c == 'z' || c == 'Z') {
        shift(chk->value, chk->width, 4, "zzzz");
      } else
      if (c >= '0' && c <= '9') {
        shift(chk->value, chk->width, 4, bitval4[c - '0']);
      } else
      if (c >= 'A' && c <= 'F') {
        shift(chk->value, chk->width, 4, bitval4[c - 'A' + 10]);
      } else
      if (c >= 'a' && c <= 'f') {
        shift(chk->value, chk->width, 4, bitval4[c - 'a' + 10]);
      } else {
        break;
      }
      c = fgetc(chkFile);
    }
  }
  c = skipSpace(c, chkFile);
  if (c != EOF) {
    ungetc(c, chkFile);
  }
  chk->next = NULL;
  return chk;
}


CHK *parseCHK(FILE *chkFile) {
  CHK *chk;
  CHK *cur;
  int c;

  chk = parseSingleChk(chkFile);
  cur = chk;
  c = fgetc(chkFile);
  while (c != EOF) {
    ungetc(c, chkFile);
    cur->next = parseSingleChk(chkFile);
    cur = cur->next;
    c = fgetc(chkFile);
  }
  return chk;
}


void showSingleCHK(CHK *chk) {
  printf("time mult = %d\n", chk->timeMult);
  printf("time unit = %s\n", tsUnit[chk->timeUnit]);
  printf("var name  = %s\n", chk->name);
  printf("var width = %d\n", chk->width);
  printf("var value = %s\n", chk->value);
}


void showCHK(CHK *chk) {
  do {
    showSingleCHK(chk);
    chk = chk->next;
    if (chk != NULL) {
      printf("&&\n");
    }
  } while (chk != NULL);
}


/**************************************************************/


#define bitEqual(v, c)	((v) == (c) || (c) == 'x')


Bool compare(Change *change, char *check, int width) {
  char *value;
  int i, n;

  if (change->vectorValue != NULL) {
    /* compare two vectors */
    value = allocate(width + 1);
    n = strlen(change->vectorValue);
    if (change->vectorValue[0] == 'x') {
      for (i = 0; i < width - n; i++) {
        value[i] = 'x';
      }
    } else
    if (change->vectorValue[0] == 'z') {
      for (i = 0; i < width - n; i++) {
        value[i] = 'z';
      }
    } else {
      for (i = 0; i < width - n; i++) {
        value[i] = '0';
      }
    }
    for (i = 0; i < n; i++) {
      value[width - n + i] = change->vectorValue[i];
    }
    value[width] = '\0';
    if (debugCheck) {
      printf("v = '%s'\nc = '%s'\n", value, check);
    }
    for (i = 0; i < width; i++) {
      if (!bitEqual(value[i], check[i])) {
        return FALSE;
      }
    }
    return TRUE;
  } else {
    /* compare two scalars */
    if (debugCheck) {
      printf("v = '%c'\nc = '%c'\n", change->scalarValue, check[0]);
    }
    return bitEqual(change->scalarValue, check[0]);
  }
}


Bool check(VCD *vcd, CHK *chk) {
  Var *var;
  int time;
  Step *step;
  Step *next;
  Change *change;

  /* locate variable */
  var = lookupVar(chk->name);
  if (var == NULL) {
    error("check: variable '%s' not found", chk->name);
  }
  /* check sizes */
  if (var->size != chk->width) {
    error("check: variable size in vcd and chk different");
  }
  /* check time units */
  if (vcd->timescaleUnit != chk->timeUnit) {
    error("check: time units in vcd and chk different");
  }
  /* locate time step just before specified time */
  time = chk->timeMult;
  step = vcd->steps;
  if (step == NULL) {
    error("check: no simulation time steps at all");
  }
  if (time < step->time) {
    error("check: check time < first simulation time step");
  }
  next = step->next;
  while (next != NULL && time >= next->time) {
    step = next;
    next = step->next;
  }
  /* find oldest time step in which the variable changed */
  while (step != NULL) {
    change = step->changes;
    while (change != NULL) {
      if (change->code == var->code) {
        /* value change found */
        return compare(change, chk->value, chk->width);
      }
      change = change->next;
    }
    step = step->prev;
  }
  error("check: variable '%s' was never dumped", chk->name);
  return FALSE;
}


/**************************************************************/


void usage(char *myself) {
  printf("Usage: %s <VCD file> <CHK file>\n", myself);
  exit(1);
}


int main(int argc, char *argv[]) {
  char *vcdName;
  FILE *vcdFile;
  VCD *vcd;
  char *chkName;
  FILE *chkFile;
  CHK *chk;

  if (argc != 3) {
    usage(argv[0]);
  }
  vcdName = argv[1];
  vcdFile = fopen(vcdName, "r");
  if (vcdFile == NULL) {
    error("cannot open VCD file '%s'", vcdName);
  }
  vcd = parseVCD(vcdFile);
  fclose(vcdFile);
  if (debugVCD) {
    showVCD(vcd);
  }
  chkName = argv[2];
  chkFile = fopen(chkName, "r");
  if (chkFile == NULL) {
    error("cannot open CHK file '%s'", chkName);
  }
  chk = parseCHK(chkFile);
  if (debugCHK) {
    showCHK(chk);
  }
  fclose(chkFile);
  while (chk != NULL) {
    if (!check(vcd, chk)) {
      printf("failure, check:\n");
      showSingleCHK(chk);
      return 1;
    }
    chk = chk->next;
  }
  printf("success\n");
  return 0;
}
