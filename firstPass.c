#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "firstPass.h"
#include "table.h"
#include "inputOutput.h"
#include "config.h"
#include "typeChecker.h"
#define NUMBER_OF_DIGITS(x) (int)(ceil(log10(x))+1)

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
    InstructionType type;
    TokenLine *tokens;
    Table *codeSymbolTable, *dataSymbolTable, *codeTable, *dataTable, *opTable;

    line = readLine(file);
    while(line != NULL){
        tokens = tokenizeLine(line, i);

        /*Check if there's a label in the line. If there was an error with the label, still set label flag for rest of line detection*/
        if(hasLabel(tokens, codeSymbolTable, dataSymbolTable) != 0)
            labelFlag = 1;

        type = getInstructType(tokens, codeSymbolTable, dataSymbolTable, labelFlag);

        /*Skip comments*/
        if(type == Comment)
            continue;
        
        /*Check if the line is a .data line*/
        if(type == Data){
            if(labelFlag)
                addLabel(tokens, dataSymbolTable, dc);
            else
                printWarning("Data declaration without a label", i);
            dc += getWordCount(tokens, Data, labelFlag);
            addData(tokens, dataTable, labelFlag);
        }

        /*Check if the line is a .string line*/
        else if(type == String){
            if(labelFlag)
                addLabel(tokens, dataSymbolTable, dc);
            else
                printWarning("String declaration without a label", i);
            dc += getWordCount(tokens, String, labelFlag);
            addString(tokens, dataTable);
        }

        /*Check if the line is a .extern line*/
        else if(type == Extern){
            addExternLabels(tokens, symbolTable);
        }

        else if(type == Op) {
            if(labelFlag)
                addLabel(tokens, codeSymbolTable, ic);
            ic += getWordCount(tokens, Op, labelFlag);
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
    connectDataTable(symbolTable, codeSymbolTable, dataSymbolTable, ic);
    return 0;
}

/*Get the number of words the data/code will take up in final file*/
getWordCount(TokenLine* tokens, InstructionType type, int labelFlag) {
    if(type == Op)
        return getOpWordCount(tokens, labelFlag);
    else if(type == Data)
        return getDataWordCount(tokens, labelFlag);
    else if(type == String)
        return getStringWordCount(tokens, labelFlag);
    else
        return 0;
}

getOpWordCount(TokenLine* tokens, int labelFlag){
    Table *group1, *group2, *group3;
    char *opName, *opParms, *parm1, *parm2, *group1Arr[] = OP_GROUP1, *group2Arr[] = OP_GROUP2, *group3Arr[] = OP_GROUP3;
    int i, opCode, opWordCount;
    if(labelFlag) {
        opName = getTokenField(1, tokens);
        opParms = getParmString(1, tokens);
    }
    else {
        opName = getTokenField(0, tokens);
        opParms = getParmString(0, tokens);
    }

    if(opName == NULL) {
        return 0;
    }
    
    group1 = createTable();
    group2 = createTable();
    group3 = createTable();

    for(i = 0; i < OP_GROUP1_COUNT; i++)
        addCell(group1Arr[i], group1);
    for(i = 0; i < OP_GROUP2_COUNT; i++)
        addCell(group2Arr[i], group2);
    for(i = 0; i < OP_GROUP3_COUNT; i++)
        addCell(group3Arr[i], group3);

    if(inTable(opName, group1)) {
        parm1 = strtok(opParms, ", \t");
        parm2 = strtok(NULL, ", \t");
        if(isRegister(parm1) && isRegister(parm2))
            opWordCount = 2;
        else
            opWordCount = 3;
    }
    else if(inTable(opName, group2))
        opWordCount = 2;
    else
        opWordCount = 1;

    free(opName);
    free(opParms);
    freeTable(group1);
    freeTable(group2);
    freeTable(group3);
    return opWordCount;
}

getDataWordCount(tokens, labelFlag) {
    char* parmString;
    int wordCount = 0;
    if(labelFlag)
        parmString = getParmString(1, tokens);
    else
        parmString = getParmString(0, tokens);
    
    if(parmString == NULL)
        return 0;
    
    /*Initilize strtok*/
    strtok(parmString, ", \t");
    while(strtok(NULL, ", \t") != NULL)
        wordCount++;
    
    free(parmString);
    /*Add one for first number*/
    return ++wordCount;
}

getStringWordCount(tokens, labelFlag) {
    char* parmString;
    int wordCount = 0;
    if(labelFlag)
        parmString = getParmString(1, tokens);
    else
        parmString = getParmString(0, tokens);
    
    if(parmString == NULL)
        return 0;
    
    /*1 word for each letter + 1 for /0 - 2 for "" */
    wordCount = strlen(parmString) - 1;
    free(parmString);
    return wordCount;
}

/*Add data to the data table*/
addData(TokenLine* tokens, Table* dataTable, int labelFlag) {
    char *parmString, *parm, *cellName, *cellData;
    int num, lastCell;
    if(labelFlag)
        parmString = getParmString(1, tokens);
    else
        parmString = getParmString(0, tokens);

    if(parmString == NULL)
        return EXIT_FAILURE;

    printf("Parm is %s\n", parmString);

    /*Get last cell name*/
    lastCell = getTableSize(dataTable);

    /*Initilize strtok*/
    parm = strtok(parmString, ", \t");
    cellName = malloc(sizeof(char) * (NUMBER_OF_DIGITS(lastCell) + 1));
    sprintf(cellName, "%d", lastCell);
    while(parm != NULL) {
        num = atoi(parm);
        if(num > MAX_NUM || num < MIN_NUM) {
            printError("Number out of range in data declaration", getLineNumber(tokens));
            return EXIT_FAILURE;
        }
        cellData = itob(num);
        addCell(cellName, dataTable);
        setCellData(cellName, cellData, dataTable);

        free(cellData);
        free(cellName);
        lastCell++;
        cellName = malloc(sizeof(char) * (NUMBER_OF_DIGITS(lastCell) + 1));
        sprintf(cellName, "%d", lastCell);
        parm = strtok(NULL, ", \t");
    }
    return EXIT_SUCCESS;
}
/*Add string to the data table*/
addString(TokenLine* tokens, Table* dataTable) {
    return 0;
}
/*Add an opertaion to the codeTable (without the label. It will be marked as labelname_1 and changed afterwards)*/
addOp(TokenLine* tokens, Table* codeTable) {
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
/*Connect the codeSymbolTable to the dataSymbolTable with the appropriate offsets*/
connectDataTable(Table* symbolTable, Table* codeSymbolTable, Table* dataSymbolTable, int ic) {
    return 0;
}

/*Get the parameter string for .extern, .entry and .data lines*/
char* getParmString(int lastField, TokenLine* tokens) {
    char* parmString = getTokenField(++lastField, tokens), *nextField;

    if(parmString == NULL)
        return NULL;
        
    while((nextField = getTokenField(++lastField, tokens)) != NULL) {
        parmString = realloc(parmString, sizeof(char) * (strlen(parmString) + strlen(nextField)));
        if(parmString == NULL){
            printError("Memory allocation failed", getLineNumber(tokens));
            return NULL;
        }
        strcat(parmString, nextField);
        printf("Next field string is %s\n", nextField);
        free(nextField);
    }
    return parmString;
}