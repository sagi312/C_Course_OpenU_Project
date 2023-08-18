#include <stdio.h>
#include "table.h"
#include "inputOutput.h"
#include "preassembler.h"
#include "firstPass.h"
#include "secondPass.h"
#include "typeChecker.h"


int testPreAssemble(void);
int testInput(void);
int testFisrtPass(void);

int main(int argc, char* argv[]){
    FILE* file = fopen("test4.txt", "r+");

    assemble(file);

    return 0;
    /*return preAssemble(NULL);*/
}  

int assemble(FILE* file){
    Table *symbolTable, *fileTable, *externTable, *entryTable;
    FILE *asFile, *externFile, *entryFile, *obFile;
    int tableSize, i;
    char *binary, *base64;

    symbolTable = createTable();
    fileTable = createTable();
    externTable = createTable();
    entryTable = createTable();

    printf("Preassemble:\n");
    asFile = preAssemble(file);
    printf("Complete\n");
    if(asFile == NULL){
        return EXIT_FAILURE;
    }
    rewind(asFile);
    printf("File is %d\n", asFile);
    printf("First pass:\n");
    firstPass(asFile, symbolTable, fileTable);
    printf("Complete\n");
    if(hasErrors() == 0) {
        printf("Second pass:\n");
        rewind(asFile);
        secondPass(asFile, symbolTable, fileTable, externTable, entryTable);
        printf("Complete\n");
    }

    printf("Errors: %d\n", hasErrors());
    printf("FILE TABLE:\n");
    printTable(fileTable);
    printf("externTable:\n");
    printTable(externTable);
    printf("entryTable:\n");
    printTable(entryTable);

    tableSize = getTableSize(fileTable);
    for(i = 0; i < tableSize; i++){
        
        binary = getCellData(getCellName(i, fileTable), fileTable);
        base64 = base64Encode(binary);
        printf("%s\n", base64);
    }
    if(hasErrors() == 0){
        if(getTableSize(externTable) > 0){
            externFile = fopen("extern.txt", "w");
            writeFileFromTableData(externFile, externTable, 1);
            fclose(externFile);
        }
        if(getTableSize(entryTable) > 0){
            entryFile = fopen("entry.txt", "w");
            writeFileFromTableData(entryFile, entryTable, 1);
            fclose(entryFile);
        }
        if(getTableSize(fileTable) > 0){
            obFile = fopen("ob.txt", "w");
            writeFileFromTableData(obFile, fileTable, 1);
            fclose(obFile);
        }
    }

    fclose(asFile);
    freeTable(symbolTable);
    freeTable(fileTable);
    freeTable(externTable);
    freeTable(entryTable);

    return EXIT_SUCCESS;
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