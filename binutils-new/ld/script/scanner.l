%{

/*
 * scanner.l -- linker script scanner specification
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "script.h"
#include "scanner.h"
#include "parser.tab.h"
#include "../../include/a.out.h"

static int lineNumber = 1;

%}

L			[A-Za-z_\.]
D			[0-9]
H			[0-9A-Fa-f]
ID			{L}({L}|{D})*
DNUM			{D}+
HNUM			0x{H}+


%%


\#.*			{
			  /* comment: nothing returned */
			}

[\ \t\r]+		{
			  /* white space: nothing returned */
			}

\n			{
			  /* newline: nothing returned */
			  lineNumber++;
			}

\|			{
			  yylval.noVal.line = lineNumber;
			  return OR;
			}

\^			{
			  yylval.noVal.line = lineNumber;
			  return XOR;
			}

\&			{
			  yylval.noVal.line = lineNumber;
			  return AND;
			}

\<\<			{
			  yylval.noVal.line = lineNumber;
			  return LSHIFT;
			}

\>\>			{
			  yylval.noVal.line = lineNumber;
			  return RSHIFT;
			}

\~			{
			  yylval.noVal.line = lineNumber;
			  return TILDE;
			}

\+			{
			  yylval.noVal.line = lineNumber;
			  return PLUS;
			}

\-			{
			  yylval.noVal.line = lineNumber;
			  return MINUS;
			}

\*			{
			  yylval.noVal.line = lineNumber;
			  return STAR;
			}

\/			{
			  yylval.noVal.line = lineNumber;
			  return SLASH;
			}

\%			{
			  yylval.noVal.line = lineNumber;
			  return PERCENT;
			}

\=			{
			  yylval.noVal.line = lineNumber;
			  return EQU;
			}

\;			{
			  yylval.noVal.line = lineNumber;
			  return SEMIC;
			}

\(			{
			  yylval.noVal.line = lineNumber;
			  return LPAREN;
			}

\)			{
			  yylval.noVal.line = lineNumber;
			  return RPAREN;
			}

\{			{
			  yylval.noVal.line = lineNumber;
			  return LCURL;
			}

\}			{
			  yylval.noVal.line = lineNumber;
			  return RCURL;
			}

ENTRY			{
			  yylval.noVal.line = lineNumber;
			  return ENTRY;
			}

ISEG			{
			  yylval.noVal.line = lineNumber;
			  return ISEG;
			}

OSEG			{
			  yylval.noVal.line = lineNumber;
			  return OSEG;
			}

{ID}			{
			  yylval.stringVal.line = lineNumber;
			  yylval.stringVal.val =
			    (char *) memAlloc(strlen(yytext) + 1);
                          strcpy(yylval.stringVal.val, yytext);
			  return IDENT;
			}

{DNUM}			{
			  yylval.intVal.line = lineNumber;
			  yylval.intVal.val = strtoul(yytext, NULL, 0);
			  return NUMBER;
			}

{HNUM}			{
			  yylval.intVal.line = lineNumber;
			  yylval.intVal.val = strtoul(yytext, NULL, 0);
			  return NUMBER;
			}

\[(A|P|W|X)*\]		{
			  char *p;
			  yylval.intVal.line = lineNumber;
			  yylval.intVal.val = 0;
			  p = yytext + 1;
			  while (*p != ']') {
			    switch (*p) {
			      case 'A':
			        yylval.intVal.val |= SEG_ATTR_A;
			        break;
			      case 'P':
			        yylval.intVal.val |= SEG_ATTR_P;
			        break;
			      case 'W':
			        yylval.intVal.val |= SEG_ATTR_W;
			        break;
			      case 'X':
			        yylval.intVal.val |= SEG_ATTR_X;
			        break;
			    }
			    p++;
			  }
			  return ATTR;
			}

.			{
			  error("illegal character 0x%02X in line %d",
			        (unsigned char) yytext[0], lineNumber);
			}


%%


int yywrap(void) {
  return 1;
}


void showToken(int token) {
  printf("TOKEN = ");
  switch (token) {
    case 0:
      printf("-- EOF --");
      break;
    case OR:
      printf("OR in line %d", yylval.noVal.line);
      break;
    case XOR:
      printf("XOR in line %d", yylval.noVal.line);
      break;
    case AND:
      printf("AND in line %d", yylval.noVal.line);
      break;
    case LSHIFT:
      printf("LSHIFT in line %d", yylval.noVal.line);
      break;
    case RSHIFT:
      printf("RSHIFT in line %d", yylval.noVal.line);
      break;
    case TILDE:
      printf("TILDE in line %d", yylval.noVal.line);
      break;
    case PLUS:
      printf("PLUS in line %d", yylval.noVal.line);
      break;
    case MINUS:
      printf("MINUS in line %d", yylval.noVal.line);
      break;
    case STAR:
      printf("STAR in line %d", yylval.noVal.line);
      break;
    case SLASH:
      printf("SLASH in line %d", yylval.noVal.line);
      break;
    case PERCENT:
      printf("PERCENT in line %d", yylval.noVal.line);
      break;
    case EQU:
      printf("EQU in line %d", yylval.noVal.line);
      break;
    case SEMIC:
      printf("SEMIC in line %d", yylval.noVal.line);
      break;
    case LPAREN:
      printf("LPAREN in line %d", yylval.noVal.line);
      break;
    case RPAREN:
      printf("RPAREN in line %d", yylval.noVal.line);
      break;
    case LCURL:
      printf("LCURL in line %d", yylval.noVal.line);
      break;
    case RCURL:
      printf("RCURL in line %d", yylval.noVal.line);
      break;
    case ENTRY:
      printf("ENTRY in line %d", yylval.noVal.line);
      break;
    case ISEG:
      printf("ISEG in line %d", yylval.noVal.line);
      break;
    case OSEG:
      printf("OSEG in line %d", yylval.noVal.line);
      break;
    case IDENT:
      printf("IDENT in line %d, value = \"%s\"",
             yylval.stringVal.line, yylval.stringVal.val);
      break;
    case NUMBER:
      printf("NUMBER in line %d, value = 0x%08X",
             yylval.intVal.line, yylval.intVal.val);
      break;
    case ATTR:
      printf("ATTR in line %d, value = 0x%08X",
             yylval.intVal.line, yylval.intVal.val);
      break;
    default:
      /* this should never happen */
      error("unknown token %d in showToken", token);
  }
  printf("\n");
}
