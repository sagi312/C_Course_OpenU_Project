#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "firstPass.h"
#include "table.h"
#include "inputOutput.h"
#include "config.h"
#include "typeChecker.h"

firstPass(FILE* file, Table* symbolTable, Table* fileTable, int* icOut, int* dcOut) {
    int ic = OFFSET, dc = 0, i = 1, errorFlag = 0, labelFlag = 0;
    char *line;
    InstructionType type;
    TokenLine *tokens;
    Table *codeSymbolTable, *dataSymbolTable, *codeTable, *dataTable;

    codeSymbolTable = createTable();
    dataSymbolTable = createTable();
    codeTable = createTable();
    dataTable = createTable();

    line = readLine(file);
    while(line != NULL && strlen(line) > 0){
        tokens = tokenizeLine(line, i);

        /*Check if there's a label in the line. If there was an error with the label, still set label flag for rest of line detection*/
        if(hasLabel(tokens, codeSymbolTable, dataSymbolTable) != 0)
            labelFlag = 1;

        type = getInstructType(tokens, codeSymbolTable, dataSymbolTable, labelFlag);
        /*Skip comments*/
        if(type == Comment) ;
        
        /*Check if the line is a .data line*/
        else if(type == Data){
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
            addString(tokens, dataTable, labelFlag);
        }

        /*Check if the line is a .extern line*/
        else if(type == Extern){
            addExternLabels(tokens, codeSymbolTable, dataSymbolTable, labelFlag);
        }

        /*Check if the line is an op*/
        else if(type == Op) {
            if(labelFlag)
                addLabel(tokens, codeSymbolTable, ic);
            ic += getWordCount(tokens, Op, labelFlag);
            addOp(tokens, codeTable, labelFlag);
        }

        free(line);
        freeTokenLine(tokens);
        i++;
        labelFlag = 0;
        line = readLine(file);
    }

    connectSymbolTables(symbolTable, codeSymbolTable, dataSymbolTable, ic);
    connectCodeTables(fileTable, codeTable, dataTable, ic);
    *icOut = ic;
    *dcOut = dc;
    return hasErrors() ? EXIT_FAILURE : EXIT_SUCCESS;
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

    /*Get last cell name*/
    lastCell = getTableSize(dataTable);

    /*Initilize strtok*/
    parm = strtok(parmString, ", \t");
    cellName = itoa(lastCell);
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
        cellName = itoa(lastCell);
        parm = strtok(NULL, ", \t");
    }
    return EXIT_SUCCESS;
}

/*Add string to the data table*/
addString(TokenLine* tokens, Table* dataTable, int labelFlag) {
    char *parmString, *cellName, *cellData;
    int i = 1, lastCell;
    char c;
    if(labelFlag)
        parmString = getParmString(1, tokens);
    else
        parmString = getParmString(0, tokens);

    if(parmString == NULL)
        return EXIT_FAILURE;

    /*Get last cell name*/
    lastCell = getTableSize(dataTable);

    cellName = itoa(lastCell);
    /*Until i is on last ", but we want to allow " in the middle of the string so we check if the next char is a null terminator*/
    while(parmString[i+1] != '\0') {
        c = parmString[i];
        if(c > MAX_CHAR || c < MIN_CHAR) {
            printError("Char out of range in string declaration", getLineNumber(tokens));
            return EXIT_FAILURE;
        }
        cellData = itob((int)c);
        addCell(cellName, dataTable);
        setCellData(cellName, cellData, dataTable);

        free(cellData);
        free(cellName);
        lastCell++;
        cellName = itoa(lastCell);
        i++;
    }

    /*Add null terminator*/
    cellName = itoa(lastCell);
    cellData = itob((int)'\0');
    addCell(cellName, dataTable);
    setCellData(cellName, cellData, dataTable);

    return EXIT_SUCCESS;
    return 0;
}
/*Add an opertaion to the codeTable (without the label, it will be changed in secondPass)*/
addOp(TokenLine* tokens, Table* codeTable, int labelFlag) {
    Table* group1, *group2, *group3, *opCodes;
    char *group1Arr[] = OP_GROUP1, *group2Arr[] = OP_GROUP2, *group3Arr[] = OP_GROUP3, *opCodesArr[] = OP_NAMES;

    char *opName, *parmString, *src, *dest, *cellName, *cellData;
    int opCode, destAddressType, srcAddressType, i;

    if(labelFlag) {
        opName = getTokenField(1, tokens);
        parmString = getParmString(1, tokens);
    }
    else {
        opName = getTokenField(0, tokens);
        parmString = getParmString(0, tokens);
    }

    if(opName == NULL) {
        printError("Missing operation name", getLineNumber(tokens));
        return EXIT_FAILURE;
    }

    group1 = createTable();
    group2 = createTable();
    group3 = createTable();
    opCodes = createTable();

    for(i = 0; i < OP_GROUP1_COUNT; i++)
        addCell(group1Arr[i], group1);
    for(i = 0; i < OP_GROUP2_COUNT; i++)
        addCell(group2Arr[i], group2);
    for(i = 0; i < OP_GROUP3_COUNT; i++)
        addCell(group3Arr[i], group3);
    for(i = 0; i < OP_NAMES_COUNT; i++) {
        addCell(opCodesArr[i], opCodes);
        setCellData(opCodesArr[i], itoa(i), opCodes);
    }

    if(!inTable(opName, opCodes)) {
        printError("Invalid operation name", getLineNumber(tokens));
        return EXIT_FAILURE;
    }

    opCode = atoi(getCellData(opName, opCodes));
    src = NULL;
    dest = NULL;
    if(parmString != NULL)
        src = strtok(parmString, ", \t");
    if(src != NULL)
        dest = strtok(NULL, ", \t");

    /*Check number of parameters*/
    if(inTable(opName, group1) && (parmString == NULL || src == NULL || dest == NULL)){
        printError("Invalid number of parameters for operation", getLineNumber(tokens));
        return EXIT_FAILURE;
    }
    if(inTable(opName, group2) && (parmString == NULL || src == NULL || (dest != NULL && strlen(dest) != 0))){
        printError("Invalid number of parameters for operation", getLineNumber(tokens));
        return EXIT_FAILURE;
    }
    if(inTable(opName, group3) && ((src != NULL && strlen(src) != 0) || (dest != NULL && strlen(dest) != 0))){
        printError("Invalid number of parameters for operation", getLineNumber(tokens));
        return EXIT_FAILURE;
    }

    /*Get addressing types*/
    if(src != NULL){
        srcAddressType = getAddresingType(src, getLineNumber(tokens));
        if(srcAddressType == -1)
            return EXIT_FAILURE;
    }
    if(dest != NULL){
        destAddressType = getAddresingType(dest, getLineNumber(tokens));
        if(destAddressType == -1)
            return EXIT_FAILURE;
    }

    /*Check if the correct addresing types are used and add encodings to table*/
    if(inTable(opName, group1)){
        /*Group 1 can accept all types as src (except lea which accepts only 3) and 3,5 as dest (excpet cmp which accepts all)*/
        if(!strcmp(opName, "lea") && srcAddressType != 3){
            printError("Invalid addressing type for operation", getLineNumber(tokens));
            return EXIT_FAILURE;
        }
        if(strcmp(opName, "cmp") && (destAddressType == 1)){
            printError("Invalid addressing type for operation", getLineNumber(tokens));
            return EXIT_FAILURE;
        }
        addOpEncoding(opCode, srcAddressType, destAddressType, codeTable);
        if(srcAddressType == 5 && destAddressType == 5){
            addRegistersEncoding(src, dest, codeTable);
        }
        else {
            if(srcAddressType == 5)
                addRegistersEncoding(src, NULL, codeTable);
            else
                addParmEncoding(src, srcAddressType, codeTable);
            if(destAddressType == 5)
                addRegistersEncoding(NULL, dest, codeTable);
            else
                addParmEncoding(dest, destAddressType, codeTable);
        }
    }

    if(inTable(opName, group2)){
        /*Dest is supposed to be the first parm because we only have one parm*/
        dest = src;
        destAddressType = srcAddressType;

        /*Group 2 can accept 3,5 as dest (excpet prn which can accept all)*/
        if(strcmp(opName, "prn") && (destAddressType == 1)){
            printError("Invalid addressing type for operation", getLineNumber(tokens));
            return EXIT_FAILURE;
        }
        addOpEncoding(opCode, 0, destAddressType, codeTable);
        addParmEncoding(dest, destAddressType, codeTable);
    }
    
    if(inTable(opName, group3)){
        addOpEncoding(opCode, 0, 0, codeTable);
    }
    
    return 0;
}

addOpEncoding(int opCode, int srcAddressType, int destAddressType, Table* codeTable) {
    char *encoding, *cellName;
    int encodingInt, lastCell;
    /*bits 2-4 are destType, 5-8 are opCode and 9-11 are srcType (starting count from 0)*/
    encodingInt = opCode << 5;
    encodingInt += srcAddressType << 9;
    encodingInt += destAddressType << 2;
    encoding = itob(encodingInt);
    
    lastCell = getTableSize(codeTable);
    cellName = itoa(lastCell+OFFSET);
    addCell(cellName, codeTable);
    setCellData(cellName, encoding, codeTable);
    free(encoding);
    free(cellName);
    return EXIT_SUCCESS;
}

addParmEncoding(char* parm, int parmAddressType, Table* codeTable) {
    char *encoding, *cellName;
    int encodingInt, lastCell;
    
    if(parmAddressType == 1){
        /*The number is in bits 2-12. Bits 0-1 are 00 because the address is absolute*/
        encodingInt = atoi(parm) << 2;
        encoding = itob(encodingInt);
    }
    else if(parmAddressType == 3){
        /*We will encode the label later*/
        encoding = strdup(parm);
    }
    else if(parmAddressType == 5){
        /*The number of the register is the third char. Here we assume that the register is the dest register, otherwise we use addRegisterEncoding*/
        encodingInt = (int)(parm[2]-'0') << 2;
        encoding = itob(encodingInt);
    }
    
    lastCell = getTableSize(codeTable);
    cellName = itoa(lastCell+OFFSET);
    addCell(cellName, codeTable);
    setCellData(cellName, encoding, codeTable);
    free(encoding);
    free(cellName);
    return EXIT_SUCCESS;
}

addRegistersEncoding(char* src, char* dest, Table* codeTable) {
    char *encoding, *cellName;
    int encodingInt, lastCell;
    

    /*The number of the register is the third char.*/
    if(src != NULL)
        encodingInt = (int)(src[2]-'0') << 7;
    else
        encodingInt = 0;

    if(dest != NULL)
        encodingInt += (int)(dest[2]-'0') << 2;

    encoding = itob(encodingInt);
    lastCell = getTableSize(codeTable);
    cellName = itoa(lastCell+OFFSET);
    addCell(cellName, codeTable);
    setCellData(cellName, encoding, codeTable);
    free(encoding);
    free(cellName);
    return EXIT_SUCCESS;
}

getAddresingType(char* parm, int lineNumber) {
    int addresType;

    if(parm == NULL)
        return 0;
    if(isRegister(parm))
        addresType = 5;
    else if(isValidLabel(parm, lineNumber, 0))
        addresType = 3;
    else if(isNum(parm)){
        if(atoi(parm) > MAX_NUM_OP || atoi(parm) < MIN_NUM_OP) {
            printError("Number out of range in operation declaration", lineNumber);
            return -1;
        }
        addresType = 1;
    }
    else {
        printError("Invalid parameter for operation", lineNumber);
        return -1;
    }
    return addresType;
}

/*Add a label to the symbol table, assuming it is a valid label*/
addLabel(TokenLine* tokens, Table* symbolTable, int address) {
    char *label, *labelName, *labelAddress;

    label = getTokenField(0, tokens);
    if(label == NULL)
        return EXIT_FAILURE;
    
    labelName = malloc(sizeof(char) * (strlen(label)-1));
    if(labelName == NULL) {
        printError("Memory allocation failed", -1);
        return EXIT_FAILURE;
    }
    strncpy(labelName, label, strlen(label)-1);
    labelName[strlen(label)-1] = '\0';
    
    labelAddress = itoa(address);

    addCell(labelName, symbolTable);
    setCellData(labelName, labelAddress, symbolTable);
    free(label);
    free(labelName);
    free(labelAddress);
    return EXIT_SUCCESS;
}
/*Add extern labels to the symbol table*/
addExternLabels(TokenLine* tokens, Table* codeSymbolTable, Table* dataSymbolTable, int labelFlag) {
    char *parmString, *labelName;

    if(labelFlag)
        parmString = getParmString(1, tokens);
    else
        parmString = getParmString(0, tokens);

    if(parmString == NULL)
        return EXIT_FAILURE;

    /*Initilize strtok*/
    labelName = strtok(parmString, ", \t");
    while(labelName != NULL) {
        if(!isValidLabel(labelName, getLineNumber(tokens), 1)) {
            free(parmString);
            free(labelName);
            return EXIT_FAILURE;
        }

        if(inTable(labelName, codeSymbolTable) || inTable(labelName, dataSymbolTable)) {
            printError("Label already exists", getLineNumber(tokens));
            free(parmString);
            free(labelName);
            return EXIT_FAILURE;
        }
        addCell(labelName, codeSymbolTable);
        setCellData(labelName, "extern", codeSymbolTable);

        labelName = strtok(NULL, ", \t");
    }
    
    free(parmString);
    return EXIT_SUCCESS;
}

/*Connect the codeSymbolTable to the dataSymbolTable with the appropriate offsets*/
connectSymbolTables(Table* symbolTable, Table* codeSymbolTable, Table* dataSymbolTable, int ic) {
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

connectCodeTables(Table* fileTable, Table* codeTable, Table* dataTable, int ic) {
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
        free(nextField);
    }
    return parmString;
}