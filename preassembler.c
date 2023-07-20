#include <stdio.h>
#include <string.h>
#include "table.h"
#include "inputOutput.h"
#include "preassembler.h"

Table *invalidTable, *macroTable;
int preAssemble(FILE* file){
    Table* fileTable = createTable();
    TokenLine* tokens;
    FILE* output = fopen("output.txt", "w");
    char *line, *currMacro;
    int inMcro = 0;

    char* invalidWords[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop", "mcro", "endmcro", ".data", ".entry", ".extern", ".string"};
    int i;
    invalidTable = createTable();
    macroTable = createTable();
    for(i = 0; i < 22; i++) {
        addCell(invalidWords[i], invalidTable);
        setCellData(invalidWords[i], "", invalidTable);
    }

    i = 1;
    line = readLine(file);
    while(line != NULL && strlen(line) > 0){
        tokens = tokenizeLine(line, i);
        i++;
        if(isValidMacro(tokens, inMcro)){
            printf("macro name in Line: %s\n", line);
            replaceMacro(fileTable, tokens);
        }
        else if(isMacro(tokens, inMcro)){
            printf("macro start in Line: %s\n", line);
            addMacroEntry(getTokenField(1, tokens));
            currMacro = strdup(getTokenField(1, tokens));
            inMcro = 1;
        }
        else if(isEndMacro(tokens, inMcro)){
            printf("macro end in Line: %s\n", line);
            inMcro = 0;
        }
        else if(inMcro){
            printf("macro data in Line: %s\n", line);
            addMacroLine(line, currMacro);
        }
        else{
            printf("regular line in Line: %s\n", line);
            addCell(line, fileTable);
            setCellData(line, line, fileTable);
        }
        line = readLine(file);
    }

    printTable(fileTable);
    writeFileFromTableData(output, fileTable, 1);

    freeTable(fileTable);
    freeTable(invalidTable);
    freeTable(macroTable);
    free(line);
    freeTokenLine(tokens);
    fclose(output);

    return 0;
}

int isMacro(TokenLine* line, int inMcro) {
    if(!strcmp(getTokenField(0, line), "mcro")) {
        if(inMcro) {
            fprintf(stderr, "Error: Nested macros aren't supported. Line %d\n", getLineNumber(line));
            return 0;
        }
        if(getTokenField(1, line) == NULL) {
            fprintf(stderr, "Error: Macro name is missing. Line %d\n", getLineNumber(line));
            return 0;
        }
        if(getTokenField(2, line) != NULL){
            fprintf(stderr, "Error: Extra text after macro declaration. Line %d.\n", getLineNumber(line));
            return 0;
        }
        if(inTable(getTokenField(1, line), invalidTable)) {
            fprintf(stderr, "Error: Macro name is a reserved word. Line %d.\n", getLineNumber(line));
            return 0;
        }
        if(inTable(getTokenField(1, line), macroTable)) {
            fprintf(stderr, "Error: Macro name is already in use. Line %d.\n", getLineNumber(line));
            return 0;
        }
        return 1;
    }    
    return 0;
}

int isEndMacro(TokenLine* line, int inMcro) {
    if(!strcmp(getTokenField(0, line), "endmcro")) {
        if(!inMcro){
            fprintf(stderr, "Error: End of macro while not in macro. Line %d.\n", getLineNumber(line));
            return 0;
        }
        if(getTokenField(1, line) != NULL) {
            fprintf(stderr, "Error: Extra text after macro ending declaration. Line %d\n", getLineNumber(line));
            return 0;
        }
        return 1;
    }
    return 0;
}

int addMacroEntry(char* name){
    addCell(name, macroTable);
    return 1;
}

int addMacroLine(char* line, char* macroName){
    char *newData, *oldData;
    oldData = getCellData(macroName, macroTable);
    if(oldData == NULL){
        newData = malloc(sizeof(char) * (strlen(line) + 1));
        if(newData == NULL){
            fprintf(stderr, "Error: Memory allocation failed.\n");
            return 0;
        }
        strcpy(newData, strip(line));
        strcat(newData, "\n");
    }
    else{
        newData = malloc(sizeof(char) * (strlen(oldData) + strlen(line) + 1));
        if(newData == NULL){
            fprintf(stderr, "Error: Memory allocation failed.\n");
            return 0;
        }
        strcpy(newData, oldData);
        strcat(newData, strip(line));
        strcat(newData, "\n");
    }
    setCellData(macroName, newData, macroTable);

    free(newData);
    free(oldData);
    return 1;
}

int replaceMacro(Table* fileTable, TokenLine* line){
    char *data, *name = getTokenField(0, line);
    data = getCellData(name, macroTable);
    addCell(name, fileTable);
    setCellData(name, data, fileTable);
    free(data);
    free(name);
    return 1;
}

int isValidMacro(TokenLine* line, int inMcro){
    if(inTable(getTokenField(0, line), macroTable)) {
        if(inMcro) {
            fprintf(stderr, "Error: Nested macros aren't supported. Line %d\n", getLineNumber(line));
            return 0;
        }
        if(getTokenField(1, line) != NULL) {
            fprintf(stderr, "Error: Extra text after macro use. Line %d\n", getLineNumber(line));
            return 0;
        }
        return 1;
    }
    
    return 0;
}