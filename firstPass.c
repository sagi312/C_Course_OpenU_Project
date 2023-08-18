#include <stdio.h>
#include "config.h"
#include "table.h"
#include "inputOutput.h"
#include "encoder.h"
#include "converter.h"
#include "typeChecker.h"

/*Local Functions*/
int getWordCount(TokenLine* tokens, InstructionType type, int labelFlag);
int connectSymbolTables(Table* symbolTable, Table* codeSymbolTable, Table* dataSymbolTable, int ic);
int connectCodeTables(Table* fileTable, Table* codeTable, Table* dataTable, int ic);
int getOpWordCount(TokenLine* tokens, int labelFlag);
int getDataWordCount(TokenLine* tokens, int labelFlag);
int getStringWordCount(TokenLine* tokens, int labelFlag);

/*This function will encode the file's content into the given fileTable. It will
  not encode the labels, and instead will save them in the symbolTable for secondPass.
  The function will also save the ic and dc counters to *icOut and *dcOut respectivly*/
int firstPass(FILE* file, Table* symbolTable, Table* fileTable, int* icOut, int* dcOut) {
    int ic = OFFSET, dc = 0, i = 1, labelFlag = 0;
    char *line;
    InstructionType type;
    TokenLine *tokens;
    Table *codeSymbolTable, *dataSymbolTable, *codeTable, *dataTable;

    codeSymbolTable = createTable(); /*A table for the code labels*/
    dataSymbolTable = createTable(); /*A table for the data labels*/
    codeTable = createTable(); /*A table for the code encodings*/
    dataTable = createTable(); /*A table for the data encodings*/

    line = readLine(file);
    while(line != NULL && strlen(line) > 0){
        tokens = tokenizeLine(line, i);

        /*Check if there's a label in the line. If there was an error with the label, still set label flag for rest of line detection*/
        if(hasLabel(tokens, codeSymbolTable, dataSymbolTable) != 0)
            labelFlag = 1;

        /*Get the instruction type*/
        type = getInstructType(tokens, codeSymbolTable, dataSymbolTable, labelFlag);

        /*Encode approprialy to instruction type*/
        switch(type) {
            case Comment:
                break;

            case Data:
                if(labelFlag)
                    addLabel(tokens, dataSymbolTable, dc);
                else
                    /*Data declaration without a label is unreachable (may be reachable with overflow)*/
                    printWarning("Data declaration without a label", i);
                dc += getWordCount(tokens, Data, labelFlag);
                addData(tokens, dataTable, labelFlag);
                break;

            case String:
                if(labelFlag)
                    addLabel(tokens, dataSymbolTable, dc);
                else
                    /*String declaration without a label is unreachable (may be reachable with overflow)*/
                    printWarning("String declaration without a label", i);
                dc += getWordCount(tokens, String, labelFlag);
                addString(tokens, dataTable, labelFlag);
                break;

            case Extern:
                addExternLabels(tokens, codeSymbolTable, dataSymbolTable, labelFlag);
                break;
            
            case Op:
                if(labelFlag)
                    addLabel(tokens, codeSymbolTable, ic);
                ic += getWordCount(tokens, Op, labelFlag);
                addOp(tokens, codeTable, labelFlag);
                break;
            
            case Entry:
                /*We will deal with entries in the second pass*/
                break;
                
            default:
                printError("Unknown instruction type", i);
                break;
        }

        free(line);
        freeTokenLine(tokens);
        i++;
        labelFlag = 0; /*Reset labelFlag*/
        line = readLine(file);
    }

    if((ic + dc-OFFSET) > MAX_MEMORY_USE)
        printError("Assembly file will use too much memory", i);

    /*Connect the symbol tables into one table*/
    connectSymbolTables(symbolTable, codeSymbolTable, dataSymbolTable, ic);
    /*Connect the code/data tables into one table*/
    connectCodeTables(fileTable, codeTable, dataTable, ic);
    /*Save the ic and dc*/
    *icOut = ic;
    *dcOut = dc;

    /*Free the tables*/
    freeTable(codeSymbolTable);
    freeTable(dataSymbolTable);
    freeTable(codeTable);
    freeTable(dataTable);

    return hasErrors() ? EXIT_FAILURE : EXIT_SUCCESS;
}

/*Get the number of words the data/code will take up in final file*/
int getWordCount(TokenLine* tokens, InstructionType type, int labelFlag) {
    if(type == Op)
        return getOpWordCount(tokens, labelFlag);
    else if(type == Data)
        return getDataWordCount(tokens, labelFlag);
    else if(type == String)
        return getStringWordCount(tokens, labelFlag);
    else
        return 0;
}

/*Get the number of words the op will take up in final file*/
int getOpWordCount(TokenLine* tokens, int labelFlag){
    Table *group1, *group2, *group3;
    char *opName, *opParms, *parm1, *parm2, *group1Arr[] = OP_GROUP1, *group2Arr[] = OP_GROUP2, *group3Arr[] = OP_GROUP3;
    int i, opWordCount;
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

/*Get the number of words the data will take up in final file*/
int getDataWordCount(TokenLine* tokens, int labelFlag) {
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

/*Get the number of words the string will take up in final file*/
int getStringWordCount(TokenLine* tokens, int labelFlag) {
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

/*Connect the codeSymbolTable to the dataSymbolTable with the appropriate offsets*/
int connectSymbolTables(Table* symbolTable, Table* codeSymbolTable, Table* dataSymbolTable, int ic) {
    char *currCell, *currCellData;
    int codeSize, dataSize, i;

    codeSize = getTableSize(codeSymbolTable);
    dataSize = getTableSize(dataSymbolTable);

    for(i = 0; i < codeSize; i++) {
        currCell = getCellName(i, codeSymbolTable);
        currCellData = getCellData(currCell, codeSymbolTable);
        addCell(currCell, symbolTable);
        setCellData(currCell, currCellData, symbolTable);
    }
    for(i = 0; i < dataSize; i++) {
        currCell = getCellName(i, dataSymbolTable);
        currCellData = getCellData(currCell, dataSymbolTable);
        currCellData = itoa(atoi(currCellData) + ic);
        addCell(currCell, symbolTable);
        setCellData(currCell, currCellData, symbolTable);
    }
    return EXIT_SUCCESS;
}

/*Connect the code table to the data table and save it in the fileTable*/
int connectCodeTables(Table* fileTable, Table* codeTable, Table* dataTable, int ic) {
    char *currCell, *currCellData;
    int codeSize, dataSize, i;

    codeSize = getTableSize(codeTable);
    dataSize = getTableSize(dataTable);

    for(i = 0; i < codeSize; i++) {
        currCell = getCellName(i, codeTable);
        currCellData = getCellData(currCell, codeTable);
        addCell(currCell, fileTable);
        setCellData(currCell, currCellData, fileTable);
    }
    for(i = 0; i < dataSize; i++) {
        currCell = getCellName(i, dataTable);
        currCellData = getCellData(currCell, dataTable);
        currCell = itoa(atoi(currCell) + ic);
        addCell(currCell, fileTable);
        setCellData(currCell, currCellData, fileTable);
    }

    return EXIT_SUCCESS;
}