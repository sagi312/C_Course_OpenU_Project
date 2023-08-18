#include <stdio.h>
#include <math.h>
#include "config.h"
#include "table.h"
#include "inputOutput.h"

#define NUMBER_OF_FIELDS 5

struct TokenLine
{
    int lineNumber;
    char* fields[NUMBER_OF_FIELDS];
};

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
                return 0;
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

int freeTokenLine(TokenLine* line){
    int i;
    for(i = 0; i < NUMBER_OF_FIELDS; i++){
        if(line->fields[i] != NULL)
            free(line->fields[i]);
    }
    free(line);
    return 1;
}

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

int writeFileFromTableData(FILE* file, Table* table, int doRewind){
    int tableSize = getTableSize(table);
    int i;
    char *cellName, *cellData;

    if(doRewind)
        rewind(file);
    
    for(i = 0; i < tableSize; i++){
        cellName = getCellName(i, table);
        cellData = getCellData(cellName, table);
        if(cellData != NULL)
            fprintf(file, "%s\n", cellData);
    }
    fflush(file);
    return 1;
}

char* getTokenField(int num, TokenLine* line) {
    char* res;
    if(num < 0 || num > NUMBER_OF_FIELDS-1)
        return NULL;
    res = strdup(line->fields[num]);
    return res;
}

int getLineNumber(TokenLine* line) {
    return line->lineNumber;
}

char* strip(char* line){
    int i = 0;
    char* res;

    while(line[i] == ' ' || line[i] == '\t'){
        i++;
    }
    res = strdup(line + i);
    return res;
}

void printWarning(char* warning, int lineNumber) {
    if(lineNumber == -1)
        fprintf(stderr, "Warning: %s\n", warning);
    else
        fprintf(stderr, "Warning in line %d: %s.\n", lineNumber, warning);
}

/*Use to keep track if there was an error printed*/
int errorFlag = 0;

void printError(char* error, int lineNumber) {
    errorFlag = 1;
    if(lineNumber == -1)
        fprintf(stderr, "Error: %s\n", error);
    else
        fprintf(stderr, "Error in line %d: %s.\n", lineNumber, error);
}

int hasErrors(void) {
    return errorFlag;
}

char* itob(int num){
    char *res = malloc((BINARY_WORD_SIZE+1)*sizeof(char));
    int i = 0;

    if(num < 0) {
        /*Invert the number*/
        num = num * -1;

        for(i = 0; i < BINARY_WORD_SIZE; i++){
            /*11-i because we're starting from the least segnificent digit, and we want to print the inverted number*/
            if(num & 1 == 1)
                res[BINARY_WORD_SIZE-1-i] = '0';
            else
                res[BINARY_WORD_SIZE-1-i] = '1';
            num >>= 1;
        }
    
        /*Add 1 to the number*/
        i = 0;
        while(i < BINARY_WORD_SIZE && res[BINARY_WORD_SIZE-1-i] == '1'){
            res[BINARY_WORD_SIZE-1-i] = '0';
            i++;
        }
        if(i < 12)
            res[BINARY_WORD_SIZE-1-i] = '1';
        else
            res[BINARY_WORD_SIZE-1-i] = '0';
    }
    else {
        for(i = 0; i < BINARY_WORD_SIZE; i++){
            /*11-i because we're starting from the least segnificent digit*/
            if(num & 1 == 1)
                res[BINARY_WORD_SIZE-1-i] = '1';
            else
                res[BINARY_WORD_SIZE-1-i] = '0';
            num >>= 1;
        }
    }

    res[BINARY_WORD_SIZE] = '\0';
    return res;
}

char* itoa(int num){
    char* res;
    int size = 0;
    if(num < 0)
        size = NUMBER_OF_DIGITS(num * -1) + 2;
    else if(num != 0)
        size = NUMBER_OF_DIGITS(num) + 1;
    res = malloc(sizeof(char) * size);
    if(res == NULL) {
        printError("Memory allocation failed", -1);
        return EXIT_FAILURE;
    }
    sprintf(res, "%d", num);
    return res;
}