/*
 * script.c -- linker script representation
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "script.h"


/**************************************************************/


ScriptNode *newStmList(ScriptNode *head, ScriptNode *tail) {
  ScriptNode *node;

  node = (ScriptNode *) memAlloc(sizeof(ScriptNode));
  node->type = NODE_STMLIST;
  node->line = -1;
  node->u.stmList.head = head;
  node->u.stmList.tail = tail;
  return node;
}


ScriptNode *newEntryStm(int line, char *name) {
  ScriptNode *node;

  node = (ScriptNode *) memAlloc(sizeof(ScriptNode));
  node->type = NODE_ENTRYSTM;
  node->line = line;
  node->u.entryStm.name = name;
  return node;
}


ScriptNode *newIsegStm(int line, char *name) {
  ScriptNode *node;

  node = (ScriptNode *) memAlloc(sizeof(ScriptNode));
  node->type = NODE_ISEGSTM;
  node->line = line;
  node->u.isegStm.name = name;
  return node;
}


ScriptNode *newOsegStm(int line, char *name,
                       unsigned int attr, ScriptNode *stms) {
  ScriptNode *node;

  node = (ScriptNode *) memAlloc(sizeof(ScriptNode));
  node->type = NODE_OSEGSTM;
  node->line = line;
  node->u.osegStm.name = name;
  node->u.osegStm.attr = attr;
  node->u.osegStm.stms = stms;
  return node;
}


ScriptNode *newAssignStm(int line, char *name, ScriptNode *exp) {
  ScriptNode *node;

  node = (ScriptNode *) memAlloc(sizeof(ScriptNode));
  node->type = NODE_ASSIGNSTM;
  node->line = line;
  node->u.assignStm.name = name;
  node->u.assignStm.exp = exp;
  return node;
}


ScriptNode *newBinopExp(int line, int op,
                        ScriptNode *left, ScriptNode *right) {
  ScriptNode *node;

  node = (ScriptNode *) memAlloc(sizeof(ScriptNode));
  node->type = NODE_BINOPEXP;
  node->line = line;
  node->u.binopExp.op = op;
  node->u.binopExp.left = left;
  node->u.binopExp.right = right;
  return node;
}


ScriptNode *newUnopExp(int line, int op, ScriptNode *exp) {
  ScriptNode *node;

  node = (ScriptNode *) memAlloc(sizeof(ScriptNode));
  node->type = NODE_UNOPEXP;
  node->line = line;
  node->u.unopExp.op = op;
  node->u.unopExp.exp = exp;
  return node;
}


ScriptNode *newInt(int line, unsigned int val) {
  ScriptNode *node;

  node = (ScriptNode *) memAlloc(sizeof(ScriptNode));
  node->type = NODE_INTEXP;
  node->line = line;
  node->u.intExp.val = val;
  return node;
}


ScriptNode *newVar(int line, char *name) {
  ScriptNode *node;

  node = (ScriptNode *) memAlloc(sizeof(ScriptNode));
  node->type = NODE_VAREXP;
  node->line = line;
  node->u.varExp.name = name;
  return node;
}


/**************************************************************/


static void indent(int n) {
  int i;

  for (i = 0; i < n; i++) {
    fprintf(stderr, "  ");
  }
}


static void say(char *s) {
  fprintf(stderr, "%s", s);
}


static void sayInt(unsigned int i) {
  fprintf(stderr, "0x%08X", i);
}


static void showStmList(ScriptNode *node, int n) {
  indent(n);
  say("StmList(");
  while (node != NULL) {
    say("\n");
    showNode(node->u.stmList.head, n + 1);
    node = node->u.stmList.tail;
    if (node != NULL) {
      say(",");
    }
  }
  say(")");
}


static void showEntryStm(ScriptNode *node, int n) {
  indent(n);
  say("EntryStm(");
  say(node->u.entryStm.name);
  say(")");
}


static void showIsegStm(ScriptNode *node, int n) {
  indent(n);
  say("IsegStm(");
  say(node->u.isegStm.name);
  say(")");
}


static void showOsegStm(ScriptNode *node, int n) {
  indent(n);
  say("OsegStm(\n");
  indent(n + 1);
  say(node->u.osegStm.name);
  say(",\n");
  indent(n + 1);
  sayInt(node->u.osegStm.attr);
  say(",\n");
  showNode(node->u.osegStm.stms, n + 1);
  say(")");
}


static void showAssignStm(ScriptNode *node, int n) {
  indent(n);
  say("AssignStm(\n");
  indent(n + 1);
  say(node->u.assignStm.name);
  say(",\n");
  showNode(node->u.assignStm.exp, n + 1);
  say(")");
}


static void showBinopExp(ScriptNode *node, int n) {
  indent(n);
  say("BinopExp(\n");
  indent(n + 1);
  switch (node->u.binopExp.op) {
    case NODE_BINOP_OR:
      say("OR");
      break;
    case NODE_BINOP_XOR:
      say("XOR");
      break;
    case NODE_BINOP_AND:
      say("AND");
      break;
    case NODE_BINOP_LSH:
      say("LSH");
      break;
    case NODE_BINOP_RSH:
      say("RSH");
      break;
    case NODE_BINOP_ADD:
      say("ADD");
      break;
    case NODE_BINOP_SUB:
      say("SUB");
      break;
    case NODE_BINOP_MUL:
      say("MUL");
      break;
    case NODE_BINOP_DIV:
      say("DIV");
      break;
    case NODE_BINOP_MOD:
      say("MOD");
      break;
    default:
      /* this should never happen */
      error("unknown operator %d in showBinopExp", node->u.binopExp.op);
  }
  say(",\n");
  showNode(node->u.binopExp.left, n + 1);
  say(",\n");
  showNode(node->u.binopExp.right, n + 1);
  say(")");
}


static void showUnopExp(ScriptNode *node, int n) {
  indent(n);
  say("UnopExp(\n");
  indent(n + 1);
  switch (node->u.unopExp.op) {
    case NODE_UNOP_PLUS:
      say("PLUS");
      break;
    case NODE_UNOP_MINUS:
      say("MINUS");
      break;
    case NODE_UNOP_COMPL:
      say("COMPL");
      break;
    default:
      /* this should never happen */
      error("unknown operator %d in showUnopExp", node->u.unopExp.op);
  }
  say(",\n");
  showNode(node->u.unopExp.exp, n + 1);
  say(")");
}


static void showIntExp(ScriptNode *node, int n) {
  indent(n);
  say("IntExp(");
  sayInt(node->u.intExp.val);
  say(")");
}


static void showVarExp(ScriptNode *node, int n) {
  indent(n);
  say("VarExp(");
  say(node->u.varExp.name);
  say(")");
}


void showNode(ScriptNode *node, int indent) {
  if (node == NULL) {
    /* end of any list */
    return;
  }
  switch (node->type) {
    case NODE_STMLIST:
      showStmList(node, indent);
      break;
    case NODE_ENTRYSTM:
      showEntryStm(node, indent);
      break;
    case NODE_ISEGSTM:
      showIsegStm(node, indent);
      break;
    case NODE_OSEGSTM:
      showOsegStm(node, indent);
      break;
    case NODE_ASSIGNSTM:
      showAssignStm(node, indent);
      break;
    case NODE_BINOPEXP:
      showBinopExp(node, indent);
      break;
    case NODE_UNOPEXP:
      showUnopExp(node, indent);
      break;
    case NODE_INTEXP:
      showIntExp(node, indent);
      break;
    case NODE_VAREXP:
      showVarExp(node, indent);
      break;
    default:
      /* this should never happen */
      error("unknown node type %d in showNode", node->type);
  }
}
