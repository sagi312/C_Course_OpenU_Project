#include <stdlib.h>
#include <ctype.h>
#include "config.h"
#include "table.h"
#include "inputOutput.h"
#include "typeChecker.h"

InstructionType getInstructType(TokenLine* tokens, Table* codeSymbolTable, Table* dataSymbolTable, int labelFlag) {
    Table* opTable = createTable();
    int i;
    char* opNames[] = OP_NAMES;

    for(i = 0; i < OP_NAMES_COUNT; i++) {
        addCell(opNames[i], opTable);
    }
    if(isComment(tokens))
        return Comment;
    if(isData(tokens, labelFlag))
        return Data;
    if(isString(tokens, labelFlag))
        return String;
    if(isExtern(tokens, labelFlag))
        return Extern;
    if(isEntry(tokens, labelFlag))
        return Entry;
    if(hasOp(tokens, opTable, labelFlag))
        return Op;
    return -1;
}

/*Check if a line is a comment*/
int isComment(TokenLine* tokens) {
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
int hasLabel(TokenLine* tokens, Table* codeSymbolTable, Table* dataSymbolTable){
    char *name, *firstField = getTokenField(0, tokens);

    if(firstField == NULL)
        return 0;
    

    if(firstField[strlen(firstField) - 1] == ':') {
        
        name = malloc(sizeof(char) * (strlen(firstField)-1));
        if(name == NULL) {
            printError("Memory allocation failed", -1);
            return EXIT_FAILURE;
        }
        strncpy(name, firstField, strlen(firstField)-1);
        name[strlen(firstField)-1] = '\0';

        if(!isValidLabel(name, getLineNumber(tokens), 1)) {
            free(name);
            free(firstField);
            return -1;
        }
        if(inTable(name, codeSymbolTable) || inTable(name, dataSymbolTable)){
            printError("Label already exists", getLineNumber(tokens));
            return -1;
        }
        free(name);
        free(firstField);
        return 1;

    }
    free(firstField);
    return 0;
}

int isValidLabel(char* label, int lineNum, int printErrors) {
    int length, i;
    if(label == NULL)
        return 0;

    if(strlen(label) < 1 || strlen(label) > MAX_LABEL_LENGTH) {
        if(printErrors)
            printError("Label is too short or too long", lineNum);
        return 0;
    }
    if(!isalpha(label[0])) {
        if(printErrors)
            printError("Label must start with a letter", lineNum);
        return 0;
    }
    length = strlen(label);
    for(i = 0; i < length; i++) {
        if(!isalnum(label[i])) {
            if(printErrors)
                printError("Label must contain only letters and numbers", lineNum);
            return 0;
        }
    }

    return 1;
}

/*Check if a line has an operation*/
int hasOp(TokenLine* tokens, Table* opTable, int labelFlag){
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
int isData(TokenLine* tokens, int labelFlag){
    char *dataDeclearation, *numberString, *num;
    if(labelFlag) {
        dataDeclearation = getTokenField(1, tokens);
        numberString = getParmString(1, tokens);
    }
    else {
        dataDeclearation = getTokenField(0, tokens);
        numberString = getParmString(0, tokens);
    }

    if(dataDeclearation == NULL)
        return 0;
    if(!strcmp(dataDeclearation, ".data")) {
        if(numberString == NULL) {
            printWarning("Data declaration without data", getLineNumber(tokens));
            free(dataDeclearation);
            return 1;
        }

        num = strtok(numberString, ", \t");

        if(num == NULL) {
            printWarning("Data declaration without data", getLineNumber(tokens));
            free(dataDeclearation);
            free(numberString);
            return 0;
        }
        if(isNum(num) == 0) {
            printError("Data declaration must contain only numbers", getLineNumber(tokens));
            free(dataDeclearation);
            free(numberString);
            return 0;
        }
        while((num = strtok(NULL, ", \t")) != NULL){
            if(isNum(num) == 0) {
                printError("Data declaration must contain only numbers", getLineNumber(tokens));
                free(dataDeclearation);
                free(numberString);
                return 0;
            }
        }
        free(dataDeclearation);
        free(numberString);
        return 1;
    }
    free(dataDeclearation);
    free(numberString);
    return 0;
}

/*Check if a line is a .string declaration of data*/
int isString(TokenLine* tokens, int labelFlag) {
    char *stringDeclearation, *string;
    if(labelFlag) {
        stringDeclearation = getTokenField(1, tokens);
        string = getParmString(1, tokens);
    }
    else {
        stringDeclearation = getTokenField(0, tokens);
        string = getParmString(0, tokens);
    }

    if(stringDeclearation == NULL)
        return 0;
    if(!strcmp(stringDeclearation, ".string")) {
        if(string == NULL) {
            printWarning("String declaration without a string", getLineNumber(tokens));
            free(stringDeclearation);
            return 1;
        }

        if(!isStringParm(string)) {
            printError("Invalid string in string declaration", getLineNumber(tokens));
            free(stringDeclearation);
            free(string);
            return 0;
        }
        free(stringDeclearation);
        free(string);
        return 1;
    }
    free(stringDeclearation);
    free(string);
    return 0;
}
/*Check if the line has an extern declaration*/
int isExtern(TokenLine* tokens, int labelFlag) {
    char *externDeclearation, *label, *parmString;
    if(labelFlag) {
        externDeclearation = getTokenField(1, tokens);
        parmString = getParmString(1, tokens);
    }
    else {
        externDeclearation = getTokenField(0, tokens);
        parmString = getParmString(0, tokens);
    }

    if(externDeclearation == NULL)
        return 0;
    if(!strcmp(externDeclearation, ".extern")) {
        if(labelFlag)
            printWarning("Label before an extern declaration has no meaning", getLineNumber(tokens));
        if(parmString == NULL || strlen(parmString) < 1) {
            printWarning("Extern declaration without a label", getLineNumber(tokens));
            free(externDeclearation);
            return 1;
        }

        label = strtok(parmString, ", \t");
        while(label != NULL) {
            if(!isValidLabel(label, getLineNumber(tokens), 1)) {
                /*Errors will be printed by checking function*/
                free(externDeclearation);
                free(parmString);
                return -1;
            }
            label = strtok(NULL, ", \t");
        }

        free(externDeclearation);
        free(parmString);
        return 1;
    }
    free(externDeclearation);
    free(parmString);
    return 0;
}

int isEntry(TokenLine* tokens, int labelFlag) {
    char *entryDeclaration, *label, *parmString;
    if(labelFlag) {
        entryDeclaration = getTokenField(1, tokens);
        parmString = getParmString(1, tokens);
    }
    else {
        entryDeclaration = getTokenField(0, tokens);
        parmString = getParmString(0, tokens);
    }

    if(entryDeclaration == NULL)
        return 0;
    if(!strcmp(entryDeclaration, ".entry")) {
        if(labelFlag)
            printWarning("Label before an entry declaration has no meaning", getLineNumber(tokens));
        if(parmString == NULL || strlen(parmString) < 1) {
            printWarning("Entry declaration without a label", getLineNumber(tokens));
            free(entryDeclaration);
            return 1;
        }

        label = strtok(parmString, ", \t");
        while(label != NULL) {
            if(!isValidLabel(label, getLineNumber(tokens), 1)) {
                /*Errors will be printed by checking function*/
                free(entryDeclaration);
                free(parmString);
                return -1;
            }
            label = strtok(NULL, ", \t");
        }

        free(entryDeclaration);
        free(parmString);
        return 1;
    }
    free(entryDeclaration);
    free(parmString);
    return 0;
}

int isRegister(char* line){
    if(line == NULL || strlen(line) != 3)
        return 0;
    if(line[0] != '@' || line[1] != 'r' || line[2] < '0' || line[2] > '7' || line[3] != '\0')
        return 0;
    return 1;
}

int isNum(char* num) {
    int i = 0;
    if(num == NULL)
        return 0;
    if(num[0] == '-' || num[0] == '+')
        i++;
    for(; i < strlen(num); i++) {
        if(!isdigit(num[i]))
            return 0;
    }
    return 1;
}

int isStringParm(char* string){
    int i;
    if(string == NULL)
        return 0;
    if(string[0] != '"' || string[strlen(string)-1] != '"')
        return 0;
    for(i = 1; i < strlen(string); i++) {
        /*ASCII printable characters are represented by codes 32 to 126*/
        if((int)string[i] < 32 || (int)string[i] > 126)
            return 0;
    }
    return 1;
}


/*Get the parameter string for the instruction lines*/
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