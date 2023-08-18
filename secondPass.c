#include <stdio.h>
#include "table.h"
#include "inputOutput.h"
#include "converter.h"
#include "typeChecker.h"
#include "secondPass.h"

/*Local functions*/
int replaceLabels(Table* symbolTable, Table* fileTable, Table* externTable);
int addExternLabel(Table* symbolTable, Table* externTable, char* labelName, int address);
int setEntryTable(FILE* file, Table* symbolTable, Table* entryTable);
int addEntry(TokenLine* tokens, Table* symbolTable, Table* entryTable);


/*Perform the second pass on the file. Replace all label placeholders with label addresses and create entry and extern tables*/
int secondPass(FILE* file, Table* symbolTable, Table* fileTable, Table* externTable, Table* entryTable){

    replaceLabels(symbolTable, fileTable, externTable);
    setEntryTable(file, symbolTable, entryTable);

    return EXIT_SUCCESS;
}

/*Replace all of the label placeholders with the correct addresses, and save the extern labels to their table*/
int replaceLabels(Table* symbolTable, Table* fileTable, Table* externTable){
    int size, addressType, encodingInt, i;
    char *labelName, *labelEncoding, *labelAddress, *line;

    size = getTableSize(fileTable);
    encodingInt = 0;
    for(i = 0; i < size; i++){
        line = getCellName(i, fileTable);
        labelName = getCellData(line, fileTable);
        /*Easy way to check if it's a label or an encoded binary word is to check if first char is a 0 or 1*/
        if(labelName[0] != '0' && labelName[0] != '1'){
            if(!inTable(labelName, symbolTable)){
                printError("Label is not defined", i);
                free(line);
                free(labelName);
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
            
            free(labelEncoding);
            /*Only free if label isn't extern - if it is extern than the string is a constant*/
            if(addressType == 2){
                free(labelAddress);
            }
        }
        free(line);
        free(labelName);
    }
    
    return EXIT_SUCCESS;
}

/*Add an extern label to the extern table*/
int addExternLabel(Table* symbolTable, Table* externTable, char* labelName, int address){
    int lastCell;
    char *cellName, *cellData;
    
    lastCell = getTableSize(externTable);
    cellName = itoa(lastCell);
    /*space for label name + \t + address + \0*/
    cellData = malloc(strlen(labelName) + 2 + NUMBER_OF_DIGITS(address));

    sprintf(cellData, "%s\t%d", labelName, address);
    addCell(cellName, externTable);
    setCellData(cellName, cellData, externTable);
    free(cellData);
    free(cellName);

    return EXIT_SUCCESS;
}

/*Go over the file and set up the entry labels in the entry table*/
int setEntryTable(FILE* file, Table* symbolTable, Table* entryTable){
    int labelFlag = 0, i = 1;
    char *line;
    InstructionType type;
    TokenLine *tokens;

    line = readLine(file);
    while(line != NULL){
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
    if(line != NULL)
        free(line);
    return 0;
}

/*Add an entry label to the entry table*/
int addEntry(TokenLine* tokens, Table* symbolTable, Table* entryTable){
    int lastCell, labelFlag = 0, size;
    char *cellName, *cellData, *labelName, *labelAddress, *parmString;

    if(hasLabel(tokens, symbolTable, NULL) != 0)
            labelFlag = 1;
    
    if(labelFlag)
        parmString = getParmString(1, tokens);
    else
        parmString = getParmString(0, tokens);

    if(parmString == NULL || strlen(parmString) < 1){
        /*Warning is displayed when checking line type*/
        if(parmString != NULL)
            free(parmString);
        return EXIT_FAILURE;
    }

    labelName = strtok(parmString, " ,\t");
    while(labelName != NULL){
        if(!inTable(labelName, symbolTable)){
            printError("Label is not defined", getLineNumber(tokens));
            free(parmString);
            return EXIT_FAILURE;
        }
        labelAddress = getCellData(labelName, symbolTable);
        if(!strcmp(labelAddress, "extern")){
            printError("Label is already defined as extern", getLineNumber(tokens));
            free(labelAddress);
            free(parmString);
            return EXIT_FAILURE;
        }
        else {
            lastCell = getTableSize(entryTable);
            cellName = itoa(lastCell);
            /*space for label name + \t + address + \0*/
            size = strlen(labelName) + 2 + NUMBER_OF_DIGITS(atoi(labelAddress));
            cellData = malloc(sizeof(char) * size);
            sprintf(cellData, "%s\t%s", labelName, labelAddress);
            if(inTableData(cellData, entryTable)){
                printWarning("Label is already defined as entry", getLineNumber(tokens));
            }
            else{
                addCell(cellName, entryTable);
                setCellData(cellName, cellData, entryTable);
            }

            free(cellData);
            free(cellName);
        }
        labelName = strtok(NULL, " ,\t");

        free(labelAddress);
    }

    free(parmString);

    return EXIT_SUCCESS;
}