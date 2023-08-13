#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "firstPass.h"
#include "table.h"
#include "inputOutput.h"
#include "config.h"

/*problems  - easy way to find patterns, regex?
            - complicated data handeling with tables, memory leaks
            - table names are complicated to work with, maybe add an extra data field?
            - need connect table function in table
            - I'm going to need to write sooooo muchhhh headers god save me
            - shares some defs with preassembler, maybe another header file?
            - It's going to take 2-3 weeks*/
firstPass(FILE* file, Table* symbolTable, Table* fileTable) {
    int ic = 0, dc = 0, i = 0, errorFlag = 0, labelFlag = 0;
    char *line;
    TokenLine *tokens;
    Table *codeSymbolTable, *dataSymbolTable, *codeTable, *dataTable, *opTable;

    line = readLine(file);
    while(line != NULL){
        tokens = tokenizeLine(line, i);

        /*Skip comments*/
        if(isComment(tokens))
            continue;
        /*Check if there's a label in the line. If there was an error with the label, still set label flag for op detectaion*/
        if(hasLabel(tokens, codeSymbolTable, dataSymbolTable) != 0)
            labelFlag = 1;

        /*Check if the line is a .data line*/
        if(isData(tokens)){
            if(labelFlag)
                addLabel(tokens, dataSymbolTable, dc);
            else
                printWarning("Data declaration without a label", i);
            dc += getWordCount(tokens);
            addData(tokens, dataTable);
        }
        /*Check if the line is a .string line*/
        else if(isString(tokens)){
            if(labelFlag)
                addLabel(tokens, dataSymbolTable, dc);
            else
                printWarning("String declaration without a label", i);
            dc += getWordCount(tokens);
            addString(tokens, dataTable);
        }
        /*Check if the line is a .extern line*/
        else if(isExtern(tokens)){
                addExternLabels(tokens, symbolTable);
        }
        else{ 
            if(labelFlag)
                addLabel(tokens, codeSymbolTable, ic);
            if(hasOp(tokens, opTable, labelFlag))
                ic += getWordCount(tokens);
                addOp(tokens, dataTable);
        }
        i++;
        free(line);
        freeTokenLine(tokens);
        line = readLine(file);
    }
    if(errorFlag)
        return -1;
    symbolTable = codeSymbolTable;
    connectDataTable(symbolTable, dataSymbolTable, ic);
    connectTables(codeTable, dataTable);
}

/*Check if a line is a comment*/
isComment(TokenLine* tokens) {
    char* firstField = getTokenField(0, tokens);
    if(firstField == NULL)
        return 0;
    if(firstField[0] == ';') {
        free(firstField);
        return 1;
    }
    free(firstField);
    return 0;
}

/*Check if a line has a label*/
hasLabel(TokenLine* tokens, Table* codeSymbolTable, Table* dataSymbolTable){
    char *name, *firstField = getTokenField(0, tokens);
    int i, length;

    if(firstField == NULL)
        return 0;
    
    /*Check if the label is too short or too long*/
    if(strlen(firstField) <= 1 || strlen(firstField) > MAX_LABEL_LENGTH) {
        free(firstField);
        return 0;
    }

    if(firstField[strlen(firstField) - 1] == ':') {
        name = malloc(sizeof(char) * (strlen(firstField)-1));
        strncpy(name, firstField, strlen(firstField)-1);
        if(inTable(name, codeSymbolTable) || inTable(name, dataSymbolTable)){
            printError("Label already exists", getLineNumber(tokens));
            free(name);
            free(firstField);
            return -1;
        }
        if(!isalpha(name[0])) {
            printError("Label must start with a letter", getLineNumber(tokens));
            free(name);
            free(firstField);
            return -1;
        }
        length = strlen(name);
        for(i = 0; i < length; i++) {
            if(!isalnum(name[i])) {
                printError("Label must contain only letters and numbers", getLineNumber(tokens));
                free(name);
                free(firstField);
                return 0;
            }
            free(firstField);
            return -1;
        }
    }
    free(firstField);
    return 0;
}

/*Check if a line has an operation*/
hasOp(TokenLine* tokens, Table* opTable, int labelFlag){
    char* opName;
    if(labelFlag)
        opName = getTokenField(1, tokens);
    else
        opName = getTokenField(0, tokens);
    if(opName == NULL)
        return 0;
    if(inTable(opName, opTable)) {
        free(opName);
        return 1;
    }
    free(opName);
    return 0;
}

/*Check if a line is a .data declaration of data*/
isData(TokenLine* tokens){
    return 0;
}

/*Check if a line is a .string declaration of data*/
isString(TokenLine* tokens) {
    return 0;
}
/*Check if the line has an extern declaration*/
isExtern(TokenLine* tokens) {
    return 0;
}
/*Get the number of words the data/code will take up in final file*/
getWordCount(TokenLine* tokens) {
    return 0;
}
/*Add data to the data table*/
addData(TokenLine* tokens, Table* dataTable) {
    return 0;
}
/*Add string to the data table*/
addString(TokenLine* tokens, Table* dataTable) {
    return 0;
}
/*Add a label to the symbol table*/
addLabel(TokenLine* tokens, Table* symbolTable, int address) {
    return 0;
}
/*Add extern labels to the symbol table*/
addExternLabels(TokenLine* tokens, Table* symbolTable) {
    return 0;
}
/*Add an opertaion to the codeTable (without the label. It will be marked as labelname_1 and changed afterwards)*/
addOp(TokenLine* tokens, Table* codeTable) {
    return 0;
}
/*Connect the codeSymbolTable to the dataSymbolTable with the appropriate offsets*/
connectDataTable(Table* codeSymbolTable, Table* dataSymbolTable, int ic) {
    return 0;
}
