#include <stdio.h>
#include "table.h"
#include "inputOutput.h"
#include "preassembler.h"
#include "firstPass.h"
#include "typeChecker.h"


int testPreAssemble(void);
int testInput(void);
int testFisrtPass(void);

int main(int argc, char* argv[]){
    FILE* file = fopen("test2.txt", "r");
    Table *symbolTable = createTable(), *fileTable = createTable();
    firstPass(file, symbolTable, fileTable);
    return 0;
    /*return preAssemble(NULL);*/
}  

int testFirstPass(void) {
    FILE* file = fopen("test2.txt", "r");
    Table* opTable = createTable(), *codeSymbolTable = createTable(), *dataSymbolTable = createTable(), *dataTable = createTable(), *codeTable = createTable();
    TokenLine* tokens;
    InstructionType type;
    char *opNames[] = OP_NAMES, *line;
    int i, labelFlag;

    for(i = 0; i < 16; i++){
        addCell(opNames[i], opTable);
        setCellData(opNames[i], opNames[i], opTable);
    }

    addCell("hello", codeSymbolTable);

    labelFlag = i = 0;
    line = readLine(file);
    while(line != NULL && strlen(line) > 0){
        printf("%s\n", line);
        i++;
        tokens = tokenizeLine(line, i);

        if(hasLabel(tokens, codeSymbolTable, dataSymbolTable) != 0) {
            printf("Line %d has a label.\n", i);
            labelFlag = 1;
        }
        type = getInstructType(tokens, codeSymbolTable, dataSymbolTable, labelFlag);
        if(type == Comment)
            printf("Line %d is a comment.\n", i);
        
        if(type == Op) {
            addOp(tokens, codeTable, labelFlag);
        }
        else if(type == Data) {
            addData(tokens, dataTable, labelFlag);
        }
        else if(type == String) {
            addString(tokens, dataTable, labelFlag);
        }
        else if(type == Extern)
            printf("Line %d is an extern.\n", i);

        freeTokenLine(tokens);
        free(line);
        labelFlag = 0;
        line = readLine(file);
    }

    printTable(dataTable);
    printTable(codeTable);

    return 0;
}

int testInput(void){
    FILE* file = fopen("test.txt", "r");
    char* line;
    TokenLine* tokenize;
    line = readLine(file);
    while(strlen(line) > 0 && line != NULL){
        printf("%s\n", line);
        tokenize = tokenizeLine(line, 0);
        printTokenLine(tokenize);
        freeTokenLine(tokenize);
        free(line);
        line = readLine(file);
    }

    return 0;
}

int testPreAssemble(void){
    FILE* file = fopen("test.txt", "r");
    preAssemble(file);
    return 0;
}