/*
 * readscript.c -- read a linker script
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "script.h"
#include "scanner.h"
#include "parser.h"
#include "readscript.h"


static int debugTokens = 0;


ScriptNode *readScript(FILE *scrFile) {
  FILE *yyinOld;
  int token;

  yyinOld = yyin;
  yyin = scrFile;
  if (debugTokens) {
    do {
      token = yylex();
      showToken(token);
    } while (token != 0);
    yyin = yyinOld;
    return NULL;
  }
  yyparse();
  yyin = yyinOld;
  return parserScriptTree;
}


void showScript(ScriptNode *script) {
  fprintf(stderr, "linker script:\n");
  if (script == NULL) {
    fprintf(stderr, "<empty>\n");
  } else {
    showNode(script, 0);
    fprintf(stderr, "\n");
  }
}
