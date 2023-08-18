#include <stdio.h>
#include <stdlib.h>
#include "stringUtils.h"
#include "table.h"
#include "inputOutput.h"

#define NUMBER_OF_FIELDS 5

struct TokenLine
{
    int lineNumber;
    char* fields[NUMBER_OF_FIELDS];
};

/*Read a line dynammicly from file*/
char* readLine(FILE* file){
    int i, size = 1;
    char *tmp, *buffer;
    char c;

    /*Initialize buffer*/
    
    buffer = malloc(sizeof(char));
    if(buffer == NULL){
        printError("Memory allocation failed", -1);
        return 0;
    }

    /*Read until next line or end of file*/
    for (i = 0;(c=getc(file)) != '\n' && c != EOF && c != '\r'; i++){
        if(i == size){
            tmp = realloc(buffer, (++size)*sizeof(char));
            if(tmp == NULL){
                fprintf(stderr, "Error: Memory allocation failed.\n");
                free(tmp);
                free(buffer);
                return NULL;
            }
            buffer = tmp;
        }
        buffer[i] = c;
    }
    /*skip \r. Only if file was generated in windows*/
    if(c == '\r'){
        c = getc(file);
    }
    /*Add null terminator*/
    if(i == size){
        tmp = realloc(buffer, (++size)*sizeof(char));
        if(tmp == NULL){
            printError("Memory allocation failed", -1);
            free(buffer);
            return 0;
        }
        buffer = tmp;
    }
    buffer[i] = '\0';
    
    return buffer;
}

/*Tokenize a given line*/
TokenLine* tokenizeLine(char* line, int lineNumber) {
    TokenLine* tokens = malloc(sizeof(TokenLine));
    int i = 0;
    char *tmp;
    
    if(tokens == NULL){
        printError("Memory allocation failed", -1);
        free(tokens);
        return NULL;
    }
    
    tmp = strdup(line);
    tokens->lineNumber = lineNumber;
    
    tokens->fields[0] = strdup(strtok(tmp, " \t"));
    /*Go over all fields not including the last one*/
    while(i < NUMBER_OF_FIELDS-2 && tokens->fields[i] != NULL) {
        tokens->fields[++i] = strdup(strtok(NULL, " \t"));
    }
    /*Add rest of line to last field or make rest of fields empty*/
    if(i == NUMBER_OF_FIELDS-2 && tokens->fields[i] != NULL)
        tokens->fields[++i] = strdup(strtok(NULL, ""));
    else {
        while(i < NUMBER_OF_FIELDS){
            tokens->fields[++i] = NULL;
        }
    }

    free(tmp);
    return tokens;
}

/*Free a token line*/
int freeTokenLine(TokenLine* line){
    int i;
    for(i = 0; i < NUMBER_OF_FIELDS; i++){
        if(line->fields[i] != NULL)
            free(line->fields[i]);
    }
    free(line);
    return 1;
}

/*Print a token line*/
int printTokenLine(TokenLine* line){
    int i;
    printf("Line number: %d\n", line->lineNumber);
    for(i = 0; i < NUMBER_OF_FIELDS; i++){
        if(line->fields[i] != NULL)
            printf("%d\t\t", i);
    }
    printf("\n");
    for(i = 0; i < NUMBER_OF_FIELDS; i++){
        if(line->fields[i] != NULL)
            printf("%s\t\t", line->fields[i]);
    }
    printf("\n");
    return 1;
}

/*Get the n'th (starting from 0) token field from a token line*/
char* getTokenField(int num, TokenLine* line) {
    char* res;
    if(num < 0 || num > NUMBER_OF_FIELDS-1)
        return NULL;
    res = strdup(line->fields[num]);
    return res;
}

/*Get the line number from a token field*/
int getLineNumber(TokenLine* line) {
    return line->lineNumber;
}

/*Write all the cells' data in the table to a file*/
int writeFileFromTableData(FILE* file, Table* table){
    int tableSize = getTableSize(table);
    int i;
    char *cellName, *cellData;

    
    for(i = 0; i < tableSize; i++){
        cellName = getCellName(i, table);
        cellData = getCellData(cellName, table);
        if(cellData != NULL)
            fprintf(file, "%s\n", cellData);
    }
    fflush(file);
    return 1;
}

/*Print a warning*/
void printWarning(char* warning, int lineNumber) {
    if(lineNumber == -1)
        fprintf(stderr, "Warning: %s\n", warning);
    else
        fprintf(stderr, "Warning in line %d: %s.\n", lineNumber, warning);
}

/*Use to keep track if there was an error printed*/
int errorFlag = 0;

/*Print an error*/
void printError(char* error, int lineNumber) {
    errorFlag = 1;
    if(lineNumber == -1)
        fprintf(stderr, "Error: %s\n", error);
    else
        fprintf(stderr, "Error in line %d: %s.\n", lineNumber, error);
}

/*Check if there was any errors printed*/
int hasErrors(void) {
    return errorFlag;
}

/*Reset the errors flag*/
int resetErrors(void){
    return errorFlag = 0;
}