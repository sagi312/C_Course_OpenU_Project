#include <stdio.h>
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
        fprintf(stderr, "Error: Memory allocation failed.\n");
        free(buffer);
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
            fprintf(stderr, "Error: Memory allocation failed\n");
            free(tmp);
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
        fprintf(stderr, "Error: Memory allocation failed.\n");
        free(tokens);
        return NULL;
    }

    tmp = strdup(line);
    tokens->lineNumber = lineNumber;

    tokens->fields[0] = strdup(strtok(tmp, " \t"));
    while(i < NUMBER_OF_FIELDS && tokens->fields[i] != NULL) {
        tokens->fields[++i] = strdup(strtok(NULL, " \t"));
    }
    free(tmp);
    return tokens;
}

int freeTokenLine(TokenLine* line){
    int i;
    for(i = 0; i < NUMBER_OF_FIELDS; i++){
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
    char* cellName;
    char* cellData;

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