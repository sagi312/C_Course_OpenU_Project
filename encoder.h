#include <stdlib.h>
#include <stdio.h>
#include "inputOutput.h"
#include "table.h"
#include "converter.h"
#include "config.h"

/*Add data to the data table*/
int addData(TokenLine* tokens, Table* dataTable, int labelFlag);

/*Add string to the data table*/
int addString(TokenLine* tokens, Table* dataTable, int labelFlag);

/*Add an opertaion to the codeTable (without the label, it will be changed in secondPass)*/
int addOp(TokenLine* tokens, Table* codeTable, int labelFlag);

/*Add a label to the symbol table, assuming it is a valid label*/
int addLabel(TokenLine* tokens, Table* symbolTable, int address);

/*Add extern labels to the symbol table*/
int addExternLabels(TokenLine* tokens, Table* codeSymbolTable, Table* dataSymbolTable, int labelFlag); 

/*Get the parameter string for the instruction lines*/
char* getParmString(int lastField, TokenLine* tokens);