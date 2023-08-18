#include <stdio.h>
#include "table.h"
#include "inputOutput.h"
#include "preassembler.h"
#include "firstPass.h"
#include "secondPass.h"
#include "typeChecker.h"


int testInput(void);
int testFisrtPass(void);
char* addFileSuffix(char* name, char* suffix);
Table* encodeFileBase64(FILE* file, Table* table);

int main(int argc, char* argv[]){
    FILE* file;
    char *name;
    int i;

    if(argc < 2){
        printf("No file name given.\n");
        return EXIT_FAILURE;
    }

    for(i = 1; i < argc; i++){
        name = argv[i];
        if(name == NULL || strlen(name) < 1){
            printf("Invalid parameter.\n");
            return EXIT_FAILURE;
        }
        printf("Assembling file %s\n", name);
        assemble(name);
        if(hasErrors() == 0)
            printf("File %s assembled successfully.\n", name);
        printf("\n");
        resetErrors();
    }

    return EXIT_SUCCESS;
}  

int assemble(char* name){
    Table *symbolTable, *amTable, *fileTable, *externTable, *entryTable, *encodedTable;
    FILE *amFile, *asFile, *obFile, *entFile, *extFile;
    int tableSize, i, ic, dc;
    char *binary, *base64, *amName, *asName, *obName, *entName, *extName;

    asName = addFileSuffix(name, ".as");
    if(asName == NULL)
        return EXIT_FAILURE;
    asFile = fopen(asName, "r+");
    if(asFile == 0){
        printError("File not found", -1);
        return EXIT_FAILURE;
    }

    symbolTable = createTable();
    fileTable = createTable();
    externTable = createTable();
    entryTable = createTable();
    amTable = createTable();

    if(preAssemble(asFile, amTable) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    fclose(asFile);
    if(hasErrors() == 0) {
        amName = addFileSuffix(name, ".am");
        if(amName == NULL)
            return EXIT_FAILURE;
        amFile = fopen(amName, "w+");
        writeFileFromTableData(amFile, amTable);
        rewind(amFile);
        firstPass(amFile, symbolTable, fileTable, &ic, &dc);
        if(hasErrors() == 0) {
            rewind(asFile);
            secondPass(asFile, symbolTable, fileTable, externTable, entryTable);
        }
    }

    if(hasErrors() == 0){
        if(getTableSize(externTable) > 0){
            extName = addFileSuffix(name, ".ext");
            if(extName == NULL)
                return EXIT_FAILURE;
            extFile = fopen(extName, "w");
            writeFileFromTableData(extFile, externTable);
            fclose(extFile);
        }
        if(getTableSize(entryTable) > 0){
            entName = addFileSuffix(name, ".ent");
            if(entName == NULL)
                return EXIT_FAILURE;
            entFile = fopen(entName, "w");
            writeFileFromTableData(entFile, entryTable);
            fclose(entFile);
        }
        if(getTableSize(fileTable) > 0){
            obName = addFileSuffix(name, ".ob");
            if(obName == NULL)
                return EXIT_FAILURE;
            obFile = fopen(obName, "w");
            encodedTable = encodeFileBase64(asFile, fileTable);
            fprintf(obFile, "%d %d\n", ic-OFFSET, dc);
            writeFileFromTableData(obFile, encodedTable);
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

Table* encodeFileBase64(FILE* file, Table* table){
    Table* base64Table = createTable();
    char* lineName, *lineData, *base64;
    int i, size;

    size = getTableSize(table);

    for(i = 0; i < size; i++){
        lineName = getCellName(i, table);
        lineData = getCellData(lineName, table);
        base64 = base64Encode(lineData);
        addCell(lineName, base64Table);
        setCellData(lineName, base64, base64Table);
    }

    return base64Table;
}

char* addFileSuffix(char* name, char* suffix){
    char* newName;
    newName = malloc(strlen(name) + strlen(suffix) + 1);
    if(newName == NULL){
        printError("Memory allocation failed", -1);
        return NULL;
    }
    strcpy(newName, name);
    strcat(newName, suffix);
    return newName;
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