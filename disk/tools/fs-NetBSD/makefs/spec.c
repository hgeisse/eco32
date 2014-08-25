/*
 * spec.c -- specfile parser
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <sys/time.h>

#include "mtree.h"
#include "pack_dev.h"


#define BUFSIZE		2000
#define REPLACEPTR(x,v)	do { if ((x)) free((x)); (x) = (v); } while (0)
#define REPLACE(x)	cur->x = new->x
#define REPLACESTR(x)	REPLACEPTR(cur->x,new->x)


static int mtree_lineno = 0;		/* Current spec line number */
static int mtree_Mflag = 0;		/* Merge duplicate entries */
static int mtree_Wflag = 0;		/* Don't "whack" permissions */
static int mtree_Sflag = 0;		/* Sort entries */


static void addchild(NODE *pathparent, NODE *centry);
static void replacenode(NODE *cur, NODE *new);
static void set(char *t, NODE *ip);
static void unset(char *t, NODE *ip);


/**************************************************************/


static void mtree_err(const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
  if (mtree_lineno) {
    printf("\nfailed at line %lu of the specification",
           (u_long) mtree_lineno);
  }
  printf("\n");
  exit(1);
}


static const char *nodetype(u_int type) {
  return inotype(nodetoino(type));
}


static int strunvis(char *dst, const char *src) {
  int i;

  i = 0;
  while ((*dst++ = *src++) != '\0') {
    i++;
  }
  return i;
}


/**************************************************************/


#define	TEST(a, b, f) {							\
	if (!strcmp(a, b)) {						\
		if (clear) {						\
			if (clrp)					\
				*clrp |= (f);				\
			if (setp)					\
				*setp &= ~(f);				\
		} else {						\
			if (setp)					\
				*setp |= (f);				\
			if (clrp)					\
				*clrp &= ~(f);				\
		}							\
		break;							\
	}								\
}


/*
 * string_to_flags --
 *	Take string of arguments and return stat flags.  Return 0 on
 *	success, 1 on failure.  On failure, stringp is set to point
 *	to the offending token.
 */
int
string_to_flags(char **stringp, u_long *setp, u_long *clrp)
{
#if HAVE_STRUCT_STAT_ST_FLAGS
	int clear;
	char *string;
	char *p;
#endif

	if (setp)
		*setp = 0;
	if (clrp)
		*clrp = 0;

#if HAVE_STRUCT_STAT_ST_FLAGS
	string = *stringp;
	while ((p = strsep(&string, "\t ,")) != NULL) {
		clear = 0;
		*stringp = p;
		if (*p == '\0')
			continue;
		if (p[0] == 'n' && p[1] == 'o') {
			clear = 1;
			p += 2;
		}
		switch (p[0]) {
		case 'a':
			TEST(p, "arch", SF_ARCHIVED);
			TEST(p, "archived", SF_ARCHIVED);
			return (1);
		case 'd':
			clear = !clear;
			TEST(p, "dump", UF_NODUMP);
			return (1);
		case 'n':
				/*
				 * Support `nonodump'. Note that
				 * the state of clear is not changed.
				 */
			TEST(p, "nodump", UF_NODUMP);
			return (1);
		case 'o':
			TEST(p, "opaque", UF_OPAQUE);
			return (1);
		case 's':
			TEST(p, "sappnd", SF_APPEND);
			TEST(p, "sappend", SF_APPEND);
			TEST(p, "schg", SF_IMMUTABLE);
			TEST(p, "schange", SF_IMMUTABLE);
			TEST(p, "simmutable", SF_IMMUTABLE);
			return (1);
		case 'u':
			TEST(p, "uappnd", UF_APPEND);
			TEST(p, "uappend", UF_APPEND);
			TEST(p, "uchg", UF_IMMUTABLE);
			TEST(p, "uchange", UF_IMMUTABLE);
			TEST(p, "uimmutable", UF_IMMUTABLE);
			return (1);
		default:
			return (1);
		}
	}
#endif

	return (0);
}


/**************************************************************/


int uid_from_user(const char *name, uid_t *uid) {
  *uid = 0;
  return 0;
}


int gid_from_group(const char *name, gid_t *gid) {
  *gid = 0;
  return 0;
}


/**************************************************************/


#define	NEEDVALUE	0x01


typedef struct _key {
	const char	*name;		/* key name */
	u_int		val;		/* value */
	u_int		flags;
} KEY;


static KEY keylist[] = {
	{"cksum",	F_CKSUM,	NEEDVALUE},
	{"device",	F_DEV,		NEEDVALUE},
	{"flags",	F_FLAGS,	NEEDVALUE},
	{"gid",		F_GID,		NEEDVALUE},
	{"gname",	F_GNAME,	NEEDVALUE},
	{"ignore",	F_IGN,		0},
	{"link",	F_SLINK,	NEEDVALUE},
	{"md5",		F_MD5,		NEEDVALUE},
	{"md5digest",	F_MD5,		NEEDVALUE},
	{"mode",	F_MODE,		NEEDVALUE},
	{"nlink",	F_NLINK,	NEEDVALUE},
	{"optional",	F_OPT,		0},
	{"rmd160",	F_RMD160,	NEEDVALUE},
	{"rmd160digest",F_RMD160,	NEEDVALUE},
	{"sha1",	F_SHA1,		NEEDVALUE},
	{"sha1digest",	F_SHA1,		NEEDVALUE},
	{"sha256",	F_SHA256,	NEEDVALUE},
	{"sha256digest",F_SHA256,	NEEDVALUE},
	{"sha384",	F_SHA384,	NEEDVALUE},
	{"sha384digest",F_SHA384,	NEEDVALUE},
	{"sha512",	F_SHA512,	NEEDVALUE},
	{"sha512digest",F_SHA512,	NEEDVALUE},
	{"size",	F_SIZE,		NEEDVALUE},
	{"tags",	F_TAGS,		NEEDVALUE},
	{"time",	F_TIME,		NEEDVALUE},
	{"type",	F_TYPE,		NEEDVALUE},
	{"uid",		F_UID,		NEEDVALUE},
	{"uname",	F_UNAME,	NEEDVALUE}
};


static KEY typelist[] = {
	{"block",	F_BLOCK,	0},
	{"char",	F_CHAR,		0},
	{"dir",		F_DIR,		0},
#ifdef S_IFDOOR
	{"door",	F_DOOR,		0},
#endif
	{"fifo",	F_FIFO,		0},
	{"file",	F_FILE,		0},
	{"link",	F_LINK,		0},
	{"socket",	F_SOCK,		0},
};


int keycompare(const void *a, const void *b) {
  return strcmp(((const KEY *)a)->name, ((const KEY *)b)->name);
}


u_int parsekey(const char *name, int *needvaluep) {
	static int allbits;
	KEY *k, tmp;

	if (allbits == 0) {
		size_t i;

		for (i = 0; i < sizeof(keylist) / sizeof(KEY); i++)
			allbits |= keylist[i].val;
	}
	tmp.name = name;
	if (strcmp(name, "all") == 0)
		return (allbits);
	k = (KEY *)bsearch(&tmp, keylist, sizeof(keylist) / sizeof(KEY),
	    sizeof(KEY), keycompare);
	if (k == NULL)
		mtree_err("unknown keyword `%s'", name);

	if (needvaluep)
		*needvaluep = k->flags & NEEDVALUE ? 1 : 0;

	return k->val;
}


u_int parsetype(const char *name) {
  KEY *k, tmp;

  tmp.name = name;
  k = (KEY *) bsearch(&tmp, typelist, sizeof(typelist) / sizeof(KEY),
                      sizeof(KEY), keycompare);
  if (k == NULL) {
    mtree_err("unknown file type `%s'", name);
  }
  return k->val;
}


/**************************************************************/


#define MAX_PACK_ARGS	3


static dev_t parsedev(char *arg) {
	u_long	numbers[MAX_PACK_ARGS];
	char	*p, *ep, *dev;
	int	argc;
	pack_t	*pack;
	dev_t	result;
	const char *error = NULL;

	if ((dev = strchr(arg, ',')) != NULL) {
		*dev++='\0';
		if ((pack = pack_find(arg)) == NULL)
			mtree_err("unknown format `%s'", arg);
		argc = 0;
		while ((p = strsep(&dev, ",")) != NULL) {
			if (*p == '\0')
				mtree_err("missing number");
			numbers[argc++] = strtoul(p, &ep, 0);
			if (*ep != '\0')
				mtree_err("invalid number `%s'",
				    p);
			if (argc > MAX_PACK_ARGS)
				mtree_err("too many arguments");
		}
		if (argc < 2)
			mtree_err("not enough arguments");
		result = (*pack)(argc, numbers, &error);
		if (error != NULL)
			mtree_err("%s", error);
	} else {
		result = (dev_t)strtoul(arg, &ep, 0);
		if (*ep != '\0')
			mtree_err("invalid device `%s'", arg);
	}
	return result;
}


/**************************************************************/


/*
 * nodecmp --
 *	used as a comparison function by addchild() to control the order
 *	in which entries appear within a list of sibling nodes.	 We make
 *	directories sort after non-directories, but otherwise sort in
 *	strcmp() order.
 *
 * Keep this in sync with dcmp() in create.c.
 */
static int nodecmp(const NODE *a, const NODE *b) {
	if ((a->type & F_DIR) != 0) {
		if ((b->type & F_DIR) == 0)
			return 1;
	} else if ((b->type & F_DIR) != 0)
		return -1;
	return strcmp(a->name, b->name);
}


/**************************************************************/


NODE *spec(FILE *fp) {
	NODE *centry, *last, *pathparent, *cur;
	char *p, *e, *next;
	NODE ginfo, *root;
	char *tname, *ntname;
	size_t tnamelen, plen;
	char buf[BUFSIZE];
	int ll;

	root = NULL;
	centry = last = NULL;
	tname = NULL;
	tnamelen = 0;
	memset(&ginfo, 0, sizeof(ginfo));
	mtree_lineno = 0;
	while (fgets(buf, BUFSIZE, fp) != NULL) {
		mtree_lineno++;
		/* Delete trailing newline. */
		ll = strlen(buf);
		if (ll > 0 && buf[ll - 1] == '\n') {
		  buf[ll - 1] = '\0';
		}
		/* Skip leading whitespace. */
		for (p = buf; *p && isspace((unsigned char)*p); ++p)
			continue;

		/* If nothing but whitespace (or comment), continue. */
		if (*p == '\0' || *p == '#')
			continue;

#ifdef DEBUG
		fprintf(stderr, "line %lu: {%s}\n",
		    (u_long)mtree_lineno, p);
#endif
		/* Grab file name, "$", "set", or "unset". */
		next = buf;
		while ((p = strsep(&next, " \t")) != NULL && *p == '\0')
			continue;
		if (p == NULL)
			mtree_err("missing field");

		if (p[0] == '/') {
			if (strcmp(p + 1, "set") == 0)
				set(next, &ginfo);
			else if (strcmp(p + 1, "unset") == 0)
				unset(next, &ginfo);
			else
				mtree_err("invalid specification `%s'", p);
			continue;
		}

		if (strcmp(p, "..") == 0) {
			/* Don't go up, if haven't gone down. */
			if (root == NULL)
				goto noparent;
			if (last->type != F_DIR || last->flags & F_DONE) {
				if (last == root)
					goto noparent;
				last = last->parent;
			}
			last->flags |= F_DONE;
			continue;

noparent:		mtree_err("no parent node");
		}

		plen = strlen(p) + 1;
		if (plen > tnamelen) {
			if ((ntname = realloc(tname, plen)) == NULL)
				mtree_err("realloc: %s", strerror(errno));
			tname = ntname;
			tnamelen = plen;
		}
		if (strunvis(tname, p) == -1)
			mtree_err("strunvis failed on `%s'", p);
		p = tname;

		pathparent = NULL;
		if (strchr(p, '/') != NULL) {
			cur = root;
			for (; (e = strchr(p, '/')) != NULL; p = e+1) {
				if (p == e)
					continue;	/* handle // */
				*e = '\0';
				if (strcmp(p, ".") != 0) {
					while (cur &&
					    strcmp(cur->name, p) != 0) {
						cur = cur->next;
					}
				}
				if (cur == NULL || cur->type != F_DIR) {
					mtree_err("%s: %s", tname,
					"missing directory in specification");
				}
				*e = '/';
				pathparent = cur;
				cur = cur->child;
			}
			if (*p == '\0')
				mtree_err("%s: empty leaf element", tname);
		}

		if ((centry = calloc(1, sizeof(NODE) + strlen(p))) == NULL)
			mtree_err("%s", strerror(errno));
		*centry = ginfo;
		centry->lineno = mtree_lineno;
		strcpy(centry->name, p);
#define	MAGIC	"?*["
		if (strpbrk(p, MAGIC))
			centry->flags |= F_MAGIC;
		set(next, centry);

		if (root == NULL) {
				/*
				 * empty tree
				 */
			if (strcmp(centry->name, ".") != 0 ||
			    centry->type != F_DIR)
				mtree_err(
				    "root node must be the directory `.'");
			last = root = centry;
			root->parent = root;
		} else if (pathparent != NULL) {
				/*
				 * full path entry; add or replace
				 */
			centry->parent = pathparent;
			addchild(pathparent, centry);
			last = centry;
		} else if (strcmp(centry->name, ".") == 0) {
				/*
				 * duplicate "." entry; always replace
				 */
			replacenode(root, centry);
		} else if (last->type == F_DIR && !(last->flags & F_DONE)) {
				/*
				 * new relative child in current dir;
				 * add or replace
				 */
			centry->parent = last;
			addchild(last, centry);
			last = centry;
		} else {
				/*
				 * new relative child in parent dir
				 * (after encountering ".." entry);
				 * add or replace
				 */
			centry->parent = last->parent;
			addchild(last->parent, centry);
			last = centry;
		}
	}
	return root;
}


/*
 * addchild --
 *	Add the centry node as a child of the pathparent node.	If
 *	centry is a duplicate, call replacenode().  If centry is not
 *	a duplicate, insert it into the linked list referenced by
 *	pathparent->child.  Keep the list sorted if Sflag is set.
 */
static void addchild(NODE *pathparent, NODE *centry) {
	NODE *samename;      /* node with the same name as centry */
	NODE *replacepos;    /* if non-NULL, centry should replace this node */
	NODE *insertpos;     /* if non-NULL, centry should be inserted
			      * after this node */
	NODE *cur;           /* for stepping through the list */
	NODE *last;          /* the last node in the list */
	int cmp;

	samename = NULL;
	replacepos = NULL;
	insertpos = NULL;
	last = NULL;
	cur = pathparent->child;
	if (cur == NULL) {
		/* centry is pathparent's first and only child node so far */
		pathparent->child = centry;
		return;
	}

	/*
	 * pathparent already has at least one other child, so add the
	 * centry node to the list.
	 *
	 * We first scan through the list looking for an existing node
	 * with the same name (setting samename), and also looking
	 * for the correct position to replace or insert the new node
	 * (setting replacepos and/or insertpos).
	 */
	for (; cur != NULL; last = cur, cur = cur->next) {
		if (strcmp(centry->name, cur->name) == 0) {
			samename = cur;
		}
		if (mtree_Sflag) {
			cmp = nodecmp(centry, cur);
			if (cmp == 0) {
				replacepos = cur;
			} else if (cmp > 0) {
				insertpos = cur;
			}
		}
	}
	if (! mtree_Sflag) {
		if (samename != NULL) {
			/* replace node with same name */
			replacepos = samename;
		} else {
			/* add new node at end of list */
			insertpos = last;
		}
	}

	if (samename != NULL) {
		/*
		 * We found a node with the same name above.  Call
		 * replacenode(), which will either exit with an error,
		 * or replace the information in the samename node and
		 * free the information in the centry node.
		 */
		replacenode(samename, centry);
		if (samename == replacepos) {
			/* The just-replaced node was in the correct position */
			return;
		}
		if (samename == insertpos || samename->prev == insertpos) {
			/*
			 * We thought the new node should be just before
			 * or just after the replaced node, but that would
			 * be equivalent to just retaining the replaced node.
			 */
			return;
		}

		/*
		 * The just-replaced node is in the wrong position in
		 * the list.  This can happen if sort order depends on
		 * criteria other than the node name.
		 *
		 * Make centry point to the just-replaced node.	 Unlink
		 * the just-replaced node from the list, and allow it to
		 * be insterted in the correct position later.
		 */
		centry = samename;
		if (centry->prev)
			centry->prev->next = centry->next;
		else {
			/* centry->next is the new head of the list */
			pathparent->child = centry->next;
			assert(centry->next != NULL);
		}
		if (centry->next)
			centry->next->prev = centry->prev;
		centry->prev = NULL;
		centry->next = NULL;
	}

	if (insertpos == NULL) {
		/* insert centry at the beginning of the list */
		pathparent->child->prev = centry;
		centry->next = pathparent->child;
		centry->prev = NULL;
		pathparent->child = centry;
	} else {
		/* insert centry into the list just after insertpos */
		centry->next = insertpos->next;
		insertpos->next = centry;
		centry->prev = insertpos;
		if (centry->next)
			centry->next->prev = centry;
	}
	return;
}


static void replacenode(NODE *cur, NODE *new) {
	if (cur->type != new->type) {
		if (mtree_Mflag) {
				/*
				 * merge entries with different types; we
				 * don't want children retained in this case.
				 */
			REPLACE(type);
			free_nodes(cur->child);
			cur->child = NULL;
		} else {
			mtree_err(
			    "existing entry for `%s', type `%s'"
			    " does not match type `%s'",
			    cur->name, nodetype(cur->type),
			    nodetype(new->type));
		}
	}

	REPLACE(st_size);
	REPLACE(st_mtimespec);
	REPLACESTR(slink);
	if (cur->slink != NULL) {
		if ((cur->slink = strdup(new->slink)) == NULL)
			mtree_err("memory allocation error");
		if (strunvis(cur->slink, new->slink) == -1)
			mtree_err("strunvis failed on `%s'", new->slink);
		free(new->slink);
	}
	REPLACE(st_uid);
	REPLACE(st_gid);
	REPLACE(st_mode);
	REPLACE(st_rdev);
	REPLACE(st_flags);
	REPLACE(st_nlink);
	REPLACE(cksum);
	REPLACESTR(md5digest);
	REPLACESTR(rmd160digest);
	REPLACESTR(sha1digest);
	REPLACESTR(sha256digest);
	REPLACESTR(sha384digest);
	REPLACESTR(sha512digest);
	REPLACESTR(tags);
	REPLACE(lineno);
	REPLACE(flags);
	free(new);
}


static void set(char *t, NODE *ip) {
	int	type, value, len;
	gid_t	gid;
	uid_t	uid;
	char	*kw, *val, *md, *ep;
	void	*m;

	while ((kw = strsep(&t, "= \t")) != NULL) {
		if (*kw == '\0')
			continue;
		if (strcmp(kw, "all") == 0)
			mtree_err("invalid keyword `all'");
		ip->flags |= type = parsekey(kw, &value);
		if (!value)
			/* Just set flag bit (F_IGN and F_OPT) */
			continue;
		while ((val = strsep(&t, " \t")) != NULL && *val == '\0')
			continue;
		if (val == NULL)
			mtree_err("missing value");
		switch (type) {
		case F_CKSUM:
			ip->cksum = strtoul(val, &ep, 10);
			if (*ep)
				mtree_err("invalid checksum `%s'", val);
			break;
		case F_DEV:
			ip->st_rdev = parsedev(val);
			break;
		case F_FLAGS:
			if (strcmp("none", val) == 0)
				ip->st_flags = 0;
			else if (string_to_flags(&val, &ip->st_flags, NULL)
			    != 0)
				mtree_err("invalid flag `%s'", val);
			break;
		case F_GID:
			ip->st_gid = (gid_t)strtoul(val, &ep, 10);
			if (*ep)
				mtree_err("invalid gid `%s'", val);
			break;
		case F_GNAME:
			if (mtree_Wflag)	/* don't parse if whacking */
				break;
			if (gid_from_group(val, &gid) == -1)
				mtree_err("unknown group `%s'", val);
			ip->st_gid = gid;
			break;
		case F_MD5:
			if (val[0]=='0' && val[1]=='x')
				md=&val[2];
			else
				md=val;
			if ((ip->md5digest = strdup(md)) == NULL)
				mtree_err("memory allocation error");
			break;
		case F_MODE:
			if ((m = setmode(val)) == NULL)
				mtree_err("cannot set file mode `%s' (%s)",
				    val, strerror(errno));
			ip->st_mode = getmode(m, 0);
			free(m);
			break;
		case F_NLINK:
			ip->st_nlink = (nlink_t)strtoul(val, &ep, 10);
			if (*ep)
				mtree_err("invalid link count `%s'", val);
			break;
		case F_RMD160:
			if (val[0]=='0' && val[1]=='x')
				md=&val[2];
			else
				md=val;
			if ((ip->rmd160digest = strdup(md)) == NULL)
				mtree_err("memory allocation error");
			break;
		case F_SHA1:
			if (val[0]=='0' && val[1]=='x')
				md=&val[2];
			else
				md=val;
			if ((ip->sha1digest = strdup(md)) == NULL)
				mtree_err("memory allocation error");
			break;
		case F_SIZE:
			ip->st_size = (off_t)strtoll(val, &ep, 10);
			if (*ep)
				mtree_err("invalid size `%s'", val);
			break;
		case F_SLINK:
			if ((ip->slink = strdup(val)) == NULL)
				mtree_err("memory allocation error");
			if (strunvis(ip->slink, val) == -1)
				mtree_err("strunvis failed on `%s'", val);
			break;
		case F_TAGS:
			len = strlen(val) + 3;	/* "," + str + ",\0" */
			if ((ip->tags = malloc(len)) == NULL)
				mtree_err("memory allocation error");
			snprintf(ip->tags, len, ",%s,", val);
			break;
		case F_TIME:
			ip->st_mtimespec.tv_sec =
			    (time_t)strtoll(val, &ep, 10);
			if (*ep != '.')
				mtree_err("invalid time `%s'", val);
			val = ep + 1;
			ip->st_mtimespec.tv_nsec = strtol(val, &ep, 10);
			if (*ep)
				mtree_err("invalid time `%s'", val);
			break;
		case F_TYPE:
			ip->type = parsetype(val);
			break;
		case F_UID:
			ip->st_uid = (uid_t)strtoul(val, &ep, 10);
			if (*ep)
				mtree_err("invalid uid `%s'", val);
			break;
		case F_UNAME:
			if (mtree_Wflag)	/* don't parse if whacking */
				break;
			if (uid_from_user(val, &uid) == -1)
				mtree_err("unknown user `%s'", val);
			ip->st_uid = uid;
			break;
		case F_SHA256:
			if (val[0]=='0' && val[1]=='x')
				md=&val[2];
			else
				md=val;
			if ((ip->sha256digest = strdup(md)) == NULL)
				mtree_err("memory allocation error");
			break;
		case F_SHA384:
			if (val[0]=='0' && val[1]=='x')
				md=&val[2];
			else
				md=val;
			if ((ip->sha384digest = strdup(md)) == NULL)
				mtree_err("memory allocation error");
			break;
		case F_SHA512:
			if (val[0]=='0' && val[1]=='x')
				md=&val[2];
			else
				md=val;
			if ((ip->sha512digest = strdup(md)) == NULL)
				mtree_err("memory allocation error");
			break;
		default:
			mtree_err(
			    "set(): unsupported key type 0x%x (INTERNAL ERROR)",
			    type);
			/* NOTREACHED */
		}
	}
}


static void unset(char *t, NODE *ip) {
  char *p;

  while ((p = strsep(&t, " \t")) != NULL) {
    if (*p == '\0') {
      continue;
    }
    ip->flags &= ~parsekey(p, NULL);
  }
}


void free_nodes(NODE *root) {
  NODE *cur, *next;

  if (root == NULL) {
    return;
  }
  next = NULL;
  for (cur = root; cur != NULL; cur = next) {
    next = cur->next;
    free_nodes(cur->child);
    REPLACEPTR(cur->slink, NULL);
    REPLACEPTR(cur->md5digest, NULL);
    REPLACEPTR(cur->rmd160digest, NULL);
    REPLACEPTR(cur->sha1digest, NULL);  
    REPLACEPTR(cur->sha256digest, NULL);
    REPLACEPTR(cur->sha384digest, NULL);
    REPLACEPTR(cur->sha512digest, NULL);
    REPLACEPTR(cur->tags, NULL);
    REPLACEPTR(cur, NULL);
  }
}
