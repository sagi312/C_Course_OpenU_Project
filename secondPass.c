#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "firstPass.h"
#include "table.h"
#include "inputOutput.h"
#include "config.h"
#include "typeChecker.h"

/*Perform the second pass on the file. Replace all label placeholders with label addresses and create entry and extern tables*/
secondPass(FILE* file, Table* symbolTable, Table* fileTable, Table* externTable, Table* entryTable){

    replaceLabels(symbolTable, fileTable, externTable);
    setEntryTable(file, symbolTable, entryTable);

    return EXIT_SUCCESS;
}

/*Replace all of the label placeholders with the correct addresses, and save the extern labels to their table*/
replaceLabels(Table* symbolTable, Table* fileTable, Table* externTable){
    int size, labelAddress, addressType, encodingInt, i, lastCell;
    char *labelName, *labelEncoding, *line, *cellName, *cellData;

    size = getTableSize(fileTable);
    encodingInt = 0;
    for(i = 0; i < size; i++){
        line = getCellName(i, fileTable);
        labelName = getCellData(line, fileTable);
        /*Easy way to check if it's a label or an encoded binary word is to check if first char is a 0 or 1*/
        if(labelName[0] != '0' && labelName[0] != '1'){
            if(!inTable(labelName, symbolTable)){
                printError("Label is not defined", i);
                return EXIT_FAILURE;
            }
            labelAddress = getCellData(labelName, symbolTable);
            if(!strcmp(labelAddress, "extern")){
                /*Then label is extern and address type is extern - 01*/
                addressType = 1;
                free(labelAddress);
                labelAddress = "0";
                
                /*Add to extern table*/
                addExternLabel(symbolTable, externTable, labelName, atoi(line));
            }
            else{
                /*Then label is local and address type is relocatable*/
                addressType = 2;
            }
            /*Encoding is 0-1 bits for address type and 2-12 bits for address*/
            encodingInt = addressType;
            encodingInt += atoi(labelAddress) << 2;
            labelEncoding = itob(encodingInt);
            setCellData(line, labelEncoding, fileTable);
        }
    }
    
    return EXIT_SUCCESS;
}

/*Add an extern label to the extern table*/
addExternLabel(Table* symbolTable, Table* externTable, char* labelName, int address){
    int lastCell;
    char *cellName, *cellData;
    
    lastCell = getTableSize(externTable);
    cellName = itoa(lastCell);
    /*space for label name + \t + address + \n*/
    cellData = malloc(strlen(labelName) + 2 + NUMBER_OF_DIGITS(address));

    sprintf(cellData, "%s\t%d\n", labelName, address);
    addCell(cellName, externTable);
    setCellData(cellName, cellData, externTable);
    free(cellData);

    return EXIT_SUCCESS;
}

/*Go over the file and set up the entry labels in the entry table*/
setEntryTable(FILE* file, Table* symbolTable, Table* entryTable){
    int labelFlag = 0, i = 1;
    char *line;
    InstructionType type;
    TokenLine *tokens;

    line = readLine(file);
    while(line != NULL && strlen(line) > 0){
        tokens = tokenizeLine(line, i);

        /*Check if there's a label in the line. If there was an error with the label, still set label flag for rest of line detection
          We pass the tables as NULL because we don't want to check if the label is already declared*/
        if(hasLabel(tokens, NULL, NULL) != 0)
            labelFlag = 1;

        /*We use symbolTable as both tables*/
        type = getInstructType(tokens, symbolTable, symbolTable, labelFlag);
        /*Add only entry labels to table*/
        if(type == Entry) {
            addEntry(tokens, symbolTable, entryTable);
        }

        free(line);
        freeTokenLine(tokens);
        i++;
        labelFlag = 0;
        line = readLine(file);
    }
    return 0;
}

/*Add an entry label to the entry table*/
addEntry(TokenLine* tokens, Table* symbolTable, Table* entryTable){
    int lastCell, labelFlag = 0;
    char *cellName, *cellData, *labelName, *parmString;

    if(hasLabel(tokens, symbolTable, NULL) != 0)
            labelFlag = 1;
    
    if(labelFlag)
        parmString = getParmString(1, tokens);
    else
        parmString = getParmString(0, tokens);

    if(parmString == NULL || strlen(parmString) < 1){
        printError("No label found for entry", getLineNumber(tokens));
        return EXIT_FAILURE;
    }

    labelName = strtok(parmString, " ,\t");
    while(labelName != NULL){
        if(!inTable(labelName, symbolTable)){
            printError("Label is not defined", getLineNumber(tokens));
            return EXIT_FAILURE;
        }
        else if(getCellData(labelName, symbolTable) == "extern"){
            printError("Label is already defined as extern", getLineNumber(tokens));
            return EXIT_FAILURE;
        }
        if(inTable(labelName, entryTable)){
            printWarning("Label is already defined as entry", getLineNumber(tokens));
        }
        else {
            lastCell = getTableSize(entryTable);
            cellName = itoa(lastCell);
            /*space for label name + \t + address + \n*/
            cellData = malloc(strlen(labelName) + 2 + NUMBER_OF_DIGITS(lastCell));
            sprintf(cellData, "%s\t%s\n", labelName, getCellData(labelName, symbolTable));
            addCell(cellName, entryTable);
            setCellData(cellName, cellData, entryTable);
            free(cellData);
        }
        labelName = strtok(NULL, " ,\t");
    }

    return EXIT_SUCCESS;
}