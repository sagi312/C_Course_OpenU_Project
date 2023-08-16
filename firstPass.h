#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "table.h"
#include "inputOutput.h"
#include "typeChecker.h"
#include "config.h"

int firstPass(FILE* file, Table* symbolTable, Table* fileTable);
/*Get the number of words the data/code will take up in final file*/
int getWordCount(TokenLine* tokens, InstructionType type, int labelFlag);
/*Add data to the data table*/
int addData(TokenLine* tokens, Table* dataTable, int labelFlag);
/*Add string to the data table*/
int addString(TokenLine* tokens, Table* dataTable, int labelFlag);
/*Add a label to the symbol table*/
int addLabel(TokenLine* tokens, Table* symbolTable, int address);
addExternLabels(TokenLine* tokens, Table* symbolTable, int labelFlag);
/*Add an opertaion to the codeTable (without the label. It will be marked as labelname_1 and changed afterwards)*/
int addOp(TokenLine* tokens, Table* codeTable, int labelFlag);
int getAddresingType(char* parm, int lineNumber);
/*Connect the codeSymbolTable to the dataSymbolTable with the appropriate offsets*/
int connectDataTable(Table* symbolTable, Table* codeSymbolTable, Table* dataSymbolTable, int ic);
char* getParmString( int lastField, TokenLine* tokens);
