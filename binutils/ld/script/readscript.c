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


ScriptNode *readScript(char *fileName) {
  int token;

  yyin = fopen(fileName, "r");
  if (yyin == NULL) {
    error("cannot open linker script file '%s'", fileName);
  }
  if (debugTokens) {
    do {
      token = yylex();
      showToken(token);
    } while (token != 0);
    fclose(yyin);
    return NULL;
  }
  yyparse();
  fclose(yyin);
  return parserScriptTree;
}


void showScript(ScriptNode *script) {
  printf("linker script:\n");
  if (script == NULL) {
    printf("<empty>\n");
  } else {
    showNode(script, 0);
    printf("\n");
  }
}
