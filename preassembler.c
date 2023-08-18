#include <stdio.h>
#include <string.h>
#include <math.h>
#include "table.h"
#include "inputOutput.h"
#include "preassembler.h"
#include "config.h"
#define NUMBER_OF_DIGITS(x) (int)(ceil(log10(x))+1)

Table *reservedWordsTable, *macroTable;
FILE* preAssemble(FILE* file){
    Table* fileTable = createTable();
    TokenLine* tokens;
    FILE* output;
    char *line, *currMacro, *secondField;
    int inMcro, i;
    char* invalidWords[] = RESERVED_NAMES;

    /*Create a table of the reserved words to check if a macro name is a reserved word*/
    reservedWordsTable = createTable();
    for(i = 0; i < RESERVED_NAMES_COUNT; i++) {
        addCell(invalidWords[i], reservedWordsTable);
        setCellData(invalidWords[i], "", reservedWordsTable);
    }

    macroTable = createTable();

    /*Inititalize variables for main loop of the preassembler*/
    i = 1;
    inMcro = 0;
    line = readLine(file);
    currMacro = secondField = NULL;
    while(line != NULL && strlen(line) > 0){
        tokens = tokenizeLine(line, i);
        secondField = getTokenField(1, tokens);
        if(isValidMacro(tokens, inMcro)){
            replaceMacro(fileTable, tokens);
        }
        else if(isMacro(tokens, inMcro)){
            addMacroEntry(secondField);
            currMacro = strdup(secondField);
            inMcro = 1;
        }
        else if(isEndMacro(tokens, inMcro)){
            inMcro = 0;
        }
        else if(inMcro){
            addMacroLine(line, currMacro);
        }
        else{
            addLine(line, i, fileTable);
        }
        freeTokenLine(tokens);
        line = readLine(file);
        i++;
    }

    if(!hasErrors()) {
        output = fopen("output.txt", "w+");
        writeFileFromTableData(output, fileTable, 1);
    }

    if(secondField != NULL)
        free(secondField);
    if(currMacro != NULL){
        free(currMacro);
    }
    if(line != NULL)
        free(line);
    freeTable(fileTable);
    freeTable(reservedWordsTable);
    freeTable(macroTable);
    

    if(hasErrors())
        return NULL;

    return output;
}

int isMacro(TokenLine* line, int inMcro) {
    char* firstField = getTokenField(0, line);
    char* secondField = getTokenField(1, line);
    char* thirdField = getTokenField(2, line);
    if(!strcmp(firstField, "mcro")) {
        if(inMcro) {
            printError("Nested macros aren't supported", getLineNumber(line));
        }
        else if(secondField == NULL) {
            printError("Macro name is missing", getLineNumber(line));
        }
        else if(thirdField != NULL){
            printError("Extra text after macro declaration", getLineNumber(line));
        }
        else if(inTable(secondField, reservedWordsTable)) {
            printError("Macro name is a reserved word", getLineNumber(line));
        }
        else if(inTable(secondField, macroTable)) {
            printError("Macro name is already in use", getLineNumber(line));
        }
        else{
            free(firstField);
            free(secondField);
            free(thirdField);
            return 1;
        }
    }
    free(firstField);
    free(secondField);
    free(thirdField);    
    return 0;
}

int isEndMacro(TokenLine* line, int inMcro) {
    if(!strcmp(getTokenField(0, line), "endmcro")) {
        if(!inMcro){
            printError("End of macro while not in macro", getLineNumber(line));
            return 0;
        }
        if(getTokenField(1, line) != NULL) {
            printError("Extra text after macro ending declaration", getLineNumber(line));
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
    /*TODO: Maybe check if there is a label and adjust tabs?*/
    char *newData, *oldData, *stripedLine = strip(line);
    oldData = getCellData(macroName, macroTable);
    /*If first line in macro*/
    if(oldData == NULL){
        /*+1 for tab for readabillity (we're not savages here)*/
        newData = malloc(sizeof(char) * (strlen(line) + 1));
        if(newData == NULL){
            printError("Memory allocation failed", -1);
            return 0;
        }
        newData[0] = '\t';
        sprintf(newData, "\t%s", stripedLine);
    }
    /*If not first line in macro*/
    else{
        /*+2 for new line and tab for readabillity (we're not savages here) and \n*/
        newData = malloc(sizeof(char) * (strlen(oldData) + strlen(line) + 2));
        if(newData == NULL){
            printError("Memory allocation failed", -1);
            return 0;
        }
        sprintf(newData, "%s\n\t%s", oldData, stripedLine);
    }
    setCellData(macroName, newData, macroTable);

    free(stripedLine);
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
            printError("Nested macros aren't supported", getLineNumber(line));
            return 0;
        }
        if(getTokenField(1, line) != NULL) {
            printError("Extra text after macro use", getLineNumber(line));
            return 0;
        }
        return 1;
    }
    
    return 0;
}

int addLine(char* line, int lineNum, Table* fileTable){
    char* lineName;
    lineName = malloc(sizeof(char) * (strlen("line ") + NUMBER_OF_DIGITS(lineNum) + 1));
    sprintf(lineName, "line %d", lineNum);
    addCell(lineName, fileTable);
    setCellData(lineName, line, fileTable);
    free(lineName);
    return EXIT_SUCCESS;
}