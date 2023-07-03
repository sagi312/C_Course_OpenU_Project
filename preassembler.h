#include <stdio.h>
#include <stdbool.h>
#include "types.h"

/*Input: file pointer
Output: create .am extended file*/
int preAssemble(FILE* file);

/*Input: tokenLine
Output: 1 if line is mcro start and valid, 0 if not*/
int checkMcro(TokenLine* line, bool inMcro);

/*Input: tokenLine
Output: 1 if line is mcro end and valid, 0 if not*/
int checkEndMcro(TokenLine* line, bool inMcro);

/*Input: mcro name
Output: 1 if added to macro table, 0 if not
Action: adds new entry to the mcro table with the mcro name*/
int addMcroTableEntry(char* name);

/*Input: mcro name, line to add
Output: 1 if added to macro table, 0 if not
Action: adds the line to the last mcro table entry*/
int addMcroTableLine(char* line);

/*Input: file pointer
Output: 1 if replaced successfully, 0 if not
Action: replace last line with the mcro from the mcro table*/
int replaceMcro(FILE* file, char* name);

/*Input: tokenLine
Output: 1 if line is mcro name in table, 0 if not*/
int inMcroTable(TokenLine* line);