#include <stdio.h>
#include <string.h>
#include "table.h"
#include "inputOutput.h"
#include "config.h"

int preAssemble(FILE* file);
int isMacro(TokenLine* line, int inMcro);
int isEndMacro(TokenLine* line, int inMcro);
int addMacroEntry(char* name);
int addMacroLine(char* line, char* macroName);
int replaceMacro(Table* fileTable, TokenLine* line);
int isValidMacro(TokenLine* line, int inMcro);
int addLine(char* line, int lineNum, Table* fileTable);