/*
 * parser.h -- parser interface
 */


#ifndef _PARSER_H_
#define _PARSER_H_


extern ScriptNode *parserScriptTree;


int yyparse(void);
void yyerror(char *msg);


#endif /* _PARSER_H_ */
