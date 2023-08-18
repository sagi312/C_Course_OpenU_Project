#include <stdio.h>
#include "stringUtils.h"
#include "table.h"
#include "inputOutput.h"
#include "converter.h"
#include "typeChecker.h"
#include "encoder.h"

/*Local functions*/
int getAddresingType(char* parm, int lineNumber);
int addOpEncoding(int opCode, int srcAddressType, int destAddressType, Table* codeTable);
int addParmEncoding(char* parm, int parmAddressType, Table* codeTable);
int addRegistersEncoding(char* src, char* dest, Table* codeTable);
int freeTables(Table* table1, Table* table2, Table* table3, Table* table4);


/*Add data to the data table*/
int addData(TokenLine* tokens, Table* dataTable, int labelFlag) {
    char *parmString, *parm, *cellName, *cellData;
    int num, lastCell;

    /*Get the coorect parm string*/
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
    while(parm != NULL) {
        cellName = itoa(lastCell);
        num = atoi(parm);
        if(num > MAX_NUM || num < MIN_NUM) {
            printError("Number out of range in data declaration", getLineNumber(tokens));
            free(parmString);
            free(cellName);
            return EXIT_FAILURE;
        }
        cellData = itob(num);
        addCell(cellName, dataTable);
        setCellData(cellName, cellData, dataTable);

        free(cellData);
        free(cellName);
        lastCell++;
        parm = strtok(NULL, ", \t");
    }
    free(parmString);
    return EXIT_SUCCESS;
}

/*Add string to the data table*/
int addString(TokenLine* tokens, Table* dataTable, int labelFlag) {
    char *parmString, *cellName, *cellData;
    int i = 1, lastCell;
    char c;
    /*Get the correct parm string*/
    if(labelFlag)
        parmString = getParmString(1, tokens);
    else
        parmString = getParmString(0, tokens);

    if(parmString == NULL)
        return EXIT_FAILURE;

    /*Get last cell name*/
    lastCell = getTableSize(dataTable);

    /*Until i is on last ", but we want to allow " in the middle of the string so we check if the next char is a null terminator*/
    while(parmString[i+1] != '\0') {
        cellName = itoa(lastCell);
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
        i++;
    }
    
    /*Add null terminator*/
    cellName = itoa(lastCell);
    cellData = itob((int)'\0');
    addCell(cellName, dataTable);
    setCellData(cellName, cellData, dataTable);

    free(cellData);
    free(cellName);
    free(parmString);

    return EXIT_SUCCESS;
}

/*Add an opertaion to the codeTable (without the label, it will be changed in secondPass)*/
int addOp(TokenLine* tokens, Table* codeTable, int labelFlag) {
    /*opCodes init*/
    Table* group1, *group2, *group3, *opCodes;
    char *group1Arr[] = OP_GROUP1, *group2Arr[] = OP_GROUP2, *group3Arr[] = OP_GROUP3, *opCodesArr[] = OP_NAMES;

    /*function variables*/
    char *opName, *parmString, *src, *dest, *opCodeStr;
    int opCode, destAddressType, srcAddressType, i;

    /*Get the correct parm string*/
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
        if(parmString != NULL)
            free(parmString);
        return EXIT_FAILURE;
    }

    /*Init op codes*/
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
        opCodeStr = itoa(i);
        setCellData(opCodesArr[i], opCodeStr, opCodes);
        free(opCodeStr);
    }

    if(!inTable(opName, opCodes)) {
        printError("Invalid operation name", getLineNumber(tokens));
        if(parmString != NULL)
            free(parmString);
        free(opName);
        freeTables(group1, group2, group3, opCodes);
        return EXIT_FAILURE;
    }

    /*Get src and dest for op*/
    opCodeStr = getCellData(opName, opCodes);
    opCode = atoi(opCodeStr);
    free(opCodeStr);
    
    src = NULL;
    dest = NULL;
    if(parmString != NULL)
        src = strtok(parmString, ", \t");
    if(src != NULL)
        dest = strtok(NULL, " ,\t");

    if(dest != NULL && strtok(NULL, " ,\t") != NULL) {
        /*This means that there are 3 parameters*/
        printError("Invalid number of parameters for operation", getLineNumber(tokens));
        if(parmString != NULL)
            free(parmString);
        free(opName);
        freeTables(group1, group2, group3, opCodes);
        return EXIT_FAILURE;
    }

    /*Check number of parameters*/
    if(inTable(opName, group1) && (parmString == NULL || src == NULL || dest == NULL)){
        printError("Invalid number of parameters for operation", getLineNumber(tokens));
        if(parmString != NULL)
            free(parmString);
        free(opName);
        freeTables(group1, group2, group3, opCodes);
        return EXIT_FAILURE;
    }
    if(inTable(opName, group2) && (parmString == NULL || src == NULL || (dest != NULL && strlen(dest) != 0))){
        printError("Invalid number of parameters for operation", getLineNumber(tokens));
        if(parmString != NULL)
            free(parmString);
        free(opName);
        freeTables(group1, group2, group3, opCodes);
        return EXIT_FAILURE;
    }
    if(inTable(opName, group3) && ((src != NULL && strlen(src) != 0) || (dest != NULL && strlen(dest) != 0))){
        printError("Invalid number of parameters for operation", getLineNumber(tokens));
        if(parmString != NULL)
            free(parmString);
        free(opName);
        freeTables(group1, group2, group3, opCodes);
        return EXIT_FAILURE;
    }

    /*Get addressing types*/
    if(src != NULL){
        srcAddressType = getAddresingType(src, getLineNumber(tokens));
        if(srcAddressType == -1) {
            if(parmString != NULL)
                free(parmString);
            free(opName);
            freeTables(group1, group2, group3, opCodes);
            return EXIT_FAILURE;
        }
    }
    if(dest != NULL){
        destAddressType = getAddresingType(dest, getLineNumber(tokens));
        if(destAddressType == -1){
            if(parmString != NULL)
                free(parmString);
            free(opName);
            freeTables(group1, group2, group3, opCodes);
            return EXIT_FAILURE;
        }
    }

    /*Check if the correct addresing types are used and add encodings to table*/
    if(inTable(opName, group1)){
        /*Group 1 can accept all types as src (except lea which accepts only 3) and 3,5 as dest (excpet cmp which accepts all)*/
        if(!strcmp(opName, "lea") && srcAddressType != 3){
            printError("Invalid addressing type for operation", getLineNumber(tokens));
            if(parmString != NULL)
                free(parmString);
            free(opName);
            freeTables(group1, group2, group3, opCodes);
            return EXIT_FAILURE;
        }
        if(strcmp(opName, "cmp") && (destAddressType == 1)){
            printError("Invalid addressing type for operation", getLineNumber(tokens));
            if(parmString != NULL)
                free(parmString);
            free(opName);
            freeTables(group1, group2, group3, opCodes);
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
            if(parmString != NULL)
                free(parmString);
            free(opName);
            freeTables(group1, group2, group3, opCodes);
            return EXIT_FAILURE;
        }
        addOpEncoding(opCode, 0, destAddressType, codeTable);
        addParmEncoding(dest, destAddressType, codeTable);
    }
    
    if(inTable(opName, group3)){
        addOpEncoding(opCode, 0, 0, codeTable);
    }
    
    if(parmString != NULL)
        free(parmString);
    free(opName);
    freeTables(group1, group2, group3, opCodes);
    return EXIT_SUCCESS;
}

/*Free all tables required for opEncoding all at once*/
int freeTables(Table* table1, Table* table2, Table* table3, Table* table4){
    freeTable(table1);
    freeTable(table2);
    freeTable(table3);
    freeTable(table4);
    return EXIT_SUCCESS;
}

/*Encode the op code in the code table*/
int addOpEncoding(int opCode, int srcAddressType, int destAddressType, Table* codeTable) {
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

/*Encode the parmater code in the code table (Will not include 2 registers correctly, use next function for this)*/
int addParmEncoding(char* parm, int parmAddressType, Table* codeTable) {
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

/*Encode 2 registers code to the code table assuming they are in the correct format*/
int addRegistersEncoding(char* src, char* dest, Table* codeTable) {
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

/*Get the addresing type for the parameter (1 for numbers, 3 for labels and 5 for registers)*/
int getAddresingType(char* parm, int lineNumber) {
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
int addLabel(TokenLine* tokens, Table* symbolTable, int address) {
    char *label, *labelName, *labelAddress;

    label = getTokenField(0, tokens);
    if(label == NULL)
        return EXIT_FAILURE;
    
    labelName = malloc(sizeof(char) * (strlen(label)));
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
int addExternLabels(TokenLine* tokens, Table* codeSymbolTable, Table* dataSymbolTable, int labelFlag) {
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