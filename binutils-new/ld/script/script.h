/*
 * script.h -- linker script representation
 */


#ifndef _SCRIPT_H_
#define _SCRIPT_H_


#define NODE_STMLIST		0
#define NODE_ENTRYSTM		1
#define NODE_ISEGSTM		2
#define NODE_OSEGSTM		3
#define NODE_ASSIGNSTM		4
#define NODE_BINOPEXP		5
#define NODE_UNOPEXP		6
#define NODE_INTEXP		7
#define NODE_VAREXP		8

#define NODE_BINOP_OR		0
#define NODE_BINOP_XOR		1
#define NODE_BINOP_AND		2
#define NODE_BINOP_LSH		3
#define NODE_BINOP_RSH		4
#define NODE_BINOP_ADD		5
#define NODE_BINOP_SUB		6
#define NODE_BINOP_MUL		7
#define NODE_BINOP_DIV		8
#define NODE_BINOP_MOD		9

#define NODE_UNOP_PLUS		0
#define NODE_UNOP_MINUS		1
#define NODE_UNOP_COMPL		2


typedef struct scriptNode {
  int type;
  int line;
  union {
    struct {
      struct scriptNode *head;
      struct scriptNode *tail;
    } stmList;
    struct {
      char *name;
    } entryStm;
    struct {
      char *name;
    } isegStm;
    struct {
      char *name;
      unsigned int attr;
      struct scriptNode *stms;
    } osegStm;
    struct {
      char *name;
      struct scriptNode *exp;
    } assignStm;
    struct {
      int op;
      struct scriptNode *left;
      struct scriptNode *right;
    } binopExp;
    struct {
      int op;
      struct scriptNode *exp;
    } unopExp;
    struct {
      unsigned int val;
    } intExp;
    struct {
      char *name;
    } varExp;
  } u;
} ScriptNode;


ScriptNode *newStmList(ScriptNode *head, ScriptNode *tail);
ScriptNode *newEntryStm(int line, char *name);
ScriptNode *newIsegStm(int line, char *name);
ScriptNode *newOsegStm(int line, char *name,
                       unsigned int attr, ScriptNode *stms);
ScriptNode *newAssignStm(int line, char *name, ScriptNode *exp);
ScriptNode *newBinopExp(int line, int op,
                        ScriptNode *left, ScriptNode *right);
ScriptNode *newUnopExp(int line, int op, ScriptNode *exp);
ScriptNode *newInt(int line, unsigned int val);
ScriptNode *newVar(int line, char *name);

void showNode(ScriptNode *node, int indent);


#endif /* _SCRIPT_H_ */
