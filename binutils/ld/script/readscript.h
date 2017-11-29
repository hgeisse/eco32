/*
 * readscript.h -- read a linker script
 */


#ifndef _READSCRIPT_H_
#define _READSCRIPT_H_


#include "script.h"


ScriptNode *readScript(FILE *scrFile);
void showScript(ScriptNode *script);


#endif /* _READSCRIPT_H_ */
