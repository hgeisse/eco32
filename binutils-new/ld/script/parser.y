%{

/*
 * parser.y -- linker script parser specification
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "script.h"
#include "scanner.h"
#include "parser.h"

ScriptNode *parserScriptTree;

%}

%union {
  NoVal noVal;
  StringVal stringVal;
  IntVal intVal;
  ScriptNode *node;
}

%token	<noVal>		OR XOR AND LSHIFT RSHIFT TILDE
%token	<noVal>		PLUS MINUS STAR SLASH PERCENT
%token	<noVal>		EQU SEMIC LPAREN RPAREN LCURL RCURL
%token	<noVal>		ENTRY ISEG OSEG
%token	<stringVal>	IDENT
%token	<intVal>	NUMBER
%token	<intVal>	ATTR

%type	<node>		stms_1
%type	<node>		stm_1
%type	<node>		stms_2
%type	<node>		stm_2
%type	<node>		entry_stm
%type	<node>		iseg_stm
%type	<node>		oseg_stm
%type	<node>		assign_stm
%type	<node>		exp
%type	<node>		or_exp
%type	<node>		xor_exp
%type	<node>		and_exp
%type	<node>		shift_exp
%type	<node>		add_exp
%type	<node>		mul_exp
%type	<node>		unary_exp
%type	<node>		primary_exp

%start			script


%%


script			: stms_1
			  {
			    parserScriptTree = $1;
			  }
			;

stms_1			: /* empty */
			  {
			    $$ = NULL;
			  }
			| stm_1 stms_1
			  {
			    $$ = newStmList($1, $2);
			  }
			;

stm_1			: entry_stm
			  {
			    $$ = $1;
			  }
			| assign_stm
			  {
			    $$ = $1;
			  }
			| oseg_stm
			  {
			    $$ = $1;
			  }
			;

stms_2			: /* empty */
			  {
			    $$ = NULL;
			  }
			| stm_2 stms_2
			  {
			    $$ = newStmList($1, $2);
			  }
			;

stm_2			: entry_stm
			  {
			    $$ = $1;
			  }
			| assign_stm
			  {
			    $$ = $1;
			  }
			| iseg_stm
			  {
			    $$ = $1;
			  }
			;

entry_stm		: ENTRY IDENT SEMIC
			  {
			    $$ = newEntryStm($1.line, $2.val);
			  }
			;

iseg_stm		: ISEG IDENT SEMIC
			  {
			    $$ = newIsegStm($1.line, $2.val);
			  }
			;

oseg_stm		: OSEG IDENT ATTR LCURL stms_2 RCURL
			  {
			    $$ = newOsegStm($1.line, $2.val, $3.val, $5);
			  }
			;

assign_stm		: IDENT EQU exp SEMIC
			  {
			    $$ = newAssignStm($2.line, $1.val, $3);
			  }
			;

exp			: or_exp
			  {
			    $$ = $1;
			  }
			;

or_exp			: xor_exp
			  {
			    $$ = $1;
			  }
			| or_exp OR xor_exp
			  {
			    $$ = newBinopExp($2.line, NODE_BINOP_OR,
			                     $1, $3);
			  }
			;

xor_exp			: and_exp
			  {
			    $$ = $1;
			  }
			| xor_exp XOR and_exp
			  {
			    $$ = newBinopExp($2.line, NODE_BINOP_XOR,
			                     $1, $3);
			  }
			;

and_exp			: shift_exp
			  {
			    $$ = $1;
			  }
			| and_exp AND shift_exp
			  {
			    $$ = newBinopExp($2.line, NODE_BINOP_AND,
			                     $1, $3);
			  }
			;

shift_exp		: add_exp
			  {
			    $$ = $1;
			  }
			| shift_exp LSHIFT add_exp
			  {
			    $$ = newBinopExp($2.line, NODE_BINOP_LSH,
			                     $1, $3);
			  }
			| shift_exp RSHIFT add_exp
			  {
			    $$ = newBinopExp($2.line, NODE_BINOP_RSH,
			                     $1, $3);
			  }
			;

add_exp			: mul_exp
			  {
			    $$ = $1;
			  }
			| add_exp PLUS mul_exp
			  {
			    $$ = newBinopExp($2.line, NODE_BINOP_ADD,
			                     $1, $3);
			  }
			| add_exp MINUS mul_exp
			  {
			    $$ = newBinopExp($2.line, NODE_BINOP_SUB,
			                     $1, $3);
			  }
			;

mul_exp			: unary_exp
			  {
			    $$ = $1;
			  }
			| mul_exp STAR unary_exp
			  {
			    $$ = newBinopExp($2.line, NODE_BINOP_MUL,
			                     $1, $3);
			  }
			| mul_exp SLASH unary_exp
			  {
			    $$ = newBinopExp($2.line, NODE_BINOP_DIV,
			                     $1, $3);
			  }
			| mul_exp PERCENT unary_exp
			  {
			    $$ = newBinopExp($2.line, NODE_BINOP_MOD,
			                     $1, $3);
			  }
			;

unary_exp		: primary_exp
			  {
			    $$ = $1;
			  }
			| PLUS unary_exp
			  {
			    $$ = newUnopExp($1.line, NODE_UNOP_PLUS, $2);
			  }
			| MINUS unary_exp
			  {
			    $$ = newUnopExp($1.line, NODE_UNOP_MINUS, $2);
			  }
			| TILDE unary_exp
			  {
			    $$ = newUnopExp($1.line, NODE_UNOP_COMPL, $2);
			  }
			;

primary_exp		: NUMBER
			  {
			    $$ = newInt($1.line, $1.val);
			  }
			| IDENT
			  {
			    $$ = newVar($1.line, $1.val);
			  }
			| LPAREN exp RPAREN
			  {
			    $$ = $2;
			  }
			;


%%


void yyerror(char *msg) {
  error("%s in line %d", msg, yylval.noVal.line);
}
