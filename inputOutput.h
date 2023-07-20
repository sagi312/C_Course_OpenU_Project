#include <stdio.h>
#include "table.h"

#ifndef INPUTOUTPUT_H
#define INPUTOUTPUT_H
typedef struct TokenLine TokenLine;
#endif

char* readLine(FILE* file);
TokenLine* tokenizeLine(char* line, int lineNumber);
int writeFileFromTableData(FILE* file, Table* table, int doRewind);
char* getTokenField(int num, TokenLine* line);
int getLineNumber(TokenLine* line);
int freeTokenLine(TokenLine* line);
int printTokenLine(TokenLine* line);
int closeFile(FILE* file);
char* mystrtok(char* str, char* delim, char** rest);
char* strip(char*);