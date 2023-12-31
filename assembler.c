#include <stdio.h>
#include "config.h"
#include "table.h"
#include "inputOutput.h"
#include "converter.h"
#include "preassembler.h"
#include "firstPass.h"
#include "secondPass.h"

/*Local functions*/
char* addFileSuffix(char* name, char* suffix); 
Table* encodeTableBase64(Table* table);
int assemble(char* name);

/*This function will assemble which file given to it as an argument*/
int main(int argc, char* argv[]){
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

/*Assemble a file to .ob, .ent and .ext files*/
int assemble(char* name){
    Table *symbolTable, *amTable, *fileTable, *externTable, *entryTable, *encodedTable;
    FILE *amFile, *asFile, *obFile, *entFile, *extFile;
    int ic, dc;
    char *amName, *asName, *obName, *entName, *extName;

    /*Open file*/
    asName = addFileSuffix(name, ".as");
    if(asName == NULL)
        return EXIT_FAILURE;
    asFile = fopen(asName, "r");
    if(asFile == 0){
        printError("File not found", -1);
        return EXIT_FAILURE;
    }
    free(asName);

    symbolTable = createTable();
    fileTable = createTable();
    externTable = createTable();
    entryTable = createTable();
    amTable = createTable();

    /*Preassemble*/
    if(preAssemble(asFile, amTable) == EXIT_FAILURE){
        fclose(asFile);
        freeTable(symbolTable);
        freeTable(fileTable);
        freeTable(externTable);
        freeTable(entryTable);
        freeTable(amTable);
        return EXIT_FAILURE;
    }
    fclose(asFile);

    if(hasErrors() == 0) {

        /*Save preassembled file*/
        amName = addFileSuffix(name, ".am");
        if(amName == NULL) {
            freeTable(symbolTable);
            freeTable(fileTable);
            freeTable(externTable);
            freeTable(entryTable);
            freeTable(amTable);
            return EXIT_FAILURE;
        }
        amFile = fopen(amName, "w+");
        writeFileFromTableData(amFile, amTable);
        free(amName);
        freeTable(amTable);

        /*First pass*/
        rewind(amFile);
        firstPass(amFile, symbolTable, fileTable, &ic, &dc);

        /*Second pass*/
        if(hasErrors() == 0) {
            rewind(amFile);
            secondPass(amFile, symbolTable, fileTable, externTable, entryTable);
        }

        fclose(amFile);
    }

    freeTable(symbolTable);

    if(hasErrors() == 0){
        /*Save ext file if there are extern labels*/
        if(getTableSize(externTable) > 0){
            extName = addFileSuffix(name, ".ext");
            if(extName == NULL) {
                freeTable(entryTable);
                freeTable(externTable);
                freeTable(fileTable);
                return EXIT_FAILURE;
            }
            extFile = fopen(extName, "w");
            writeFileFromTableData(extFile, externTable);
            fclose(extFile);
            free(extName);
        }
        freeTable(externTable);
        /*Save ent file if there are entry labels*/
        if(getTableSize(entryTable) > 0){
            entName = addFileSuffix(name, ".ent");
            if(entName == NULL) {
                freeTable(entryTable);
                freeTable(fileTable);
                return EXIT_FAILURE;
            }
            entFile = fopen(entName, "w");
            writeFileFromTableData(entFile, entryTable);
            fclose(entFile);
            free(entName);
        }
        freeTable(entryTable);
        /*Save ob file*/
        obName = addFileSuffix(name, ".ob");
        if(obName == NULL) {
            freeTable(fileTable);
            return EXIT_FAILURE;
        }
        obFile = fopen(obName, "w");
        
        /*Encode table*/
        encodedTable = encodeTableBase64(fileTable);
        /*Save ic and dc to top of file*/
        fprintf(obFile, "%d %d\n", ic-OFFSET, dc);
        writeFileFromTableData(obFile, encodedTable);
        fclose(obFile);
        free(obName);
        freeTable(encodedTable);
        freeTable(fileTable);
    }
    else{
        freeTable(fileTable);
        freeTable(entryTable);
        freeTable(externTable);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/*Encode a file table to base 64 assuming it is in binary format*/
Table* encodeTableBase64(Table* table){
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
        free(lineName);
        free(lineData);
        free(base64);
    }

    return base64Table;
}

/*Add a suffix to a given file name*/
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