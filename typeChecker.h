#include <stdlib.h>
#include <ctype.h>
#include "inputOutput.h"
#include "table.h"
#include "config.h"

#ifndef TYPECHECKER_H
typedef enum InstructionType{Comment, Op, Data, String, Extern, Entry} InstructionType;
#define TYPECHECKER_H
#endif

InstructionType getInstructType(TokenLine* tokens, Table* codeSymbolTable, Table* dataSymbolTable, int labelFlag);
int isComment(TokenLine* tokens);
int hasLabel(TokenLine* tokens, Table* codeSymbolTable, Table* dataSymbolTable);
int isValidLabel(char* label, int lineNum, int printErrors);
int hasOp(TokenLine* tokens, Table* opTable, int labelFlag);
int isData(TokenLine* tokens, int labelFlag);
int isString(TokenLine* tokens, int labelFlag) ;
int isExtern(TokenLine* tokens, int labelFlag);
int isEntry(TokenLine* tokens, int labelFlag);
int isRegister(char* line);
int isNum(char* num);
int isStringParm(char* string);
