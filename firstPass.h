#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "table.h"
#include "inputOutput.h"
#include "config.h"

int firstPass(FILE* file, Table* symbolTable, Table* fileTable);
int isComment(TokenLine* tokens);
/*Check if a line has a label*/
int hasLabel(TokenLine* tokens, Table* codeSymbolTable, Table* dataSymbolTable);
/*Check if a line has an operation*/
int hasOp(TokenLine* tokens, Table* opTable, int labelFlag);
/*Check if a line is a .data declaration of data*/
int isData(TokenLine* tokens);
/*Check if a line is a .string declaration of data*/
int isString(TokenLine* tokens);
/*Check if the line has an extern declaration*/
int isExtern(TokenLine* tokens);
/*Get the number of words the data/code will take up in final file*/
int getWordCount(TokenLine* tokens);
/*Add data to the data table*/
int addData(TokenLine* tokens, Table* dataTable);
/*Add string to the data table*/
int addString(TokenLine* tokens, Table* dataTable);
/*Add a label to the symbol table*/
int addLabel(TokenLine* tokens, Table* symbolTable, int address);
/*Add an opertaion to the codeTable (without the label. It will be marked as labelname_1 and changed afterwards)*/
int addOp(TokenLine* tokens, Table* codeTable);
/*Connect the codeSymbolTable to the dataSymbolTable with the appropriate offsets*/
int connectDataTable(Table* codeSymbolTable, Table* dataSymbolTable, int ic);