#include <stdio.h>
#include "table.h"
#include "inputOutput.h"

#define NUMBER_OF_FIELDS 5

struct TokenLine
{
    int lineNumber;
    char* fields[NUMBER_OF_FIELDS];
};

FILE* openFile(char* name) {
    FILE* file = fopen(name, "r+");
    if(file == NULL) {
        file = fopen(name, "w+");
    }
    return file;
}

int closeFile(FILE* file){
    fclose(file);
    return 1;
}

char* readLine(FILE* file){
    int i, size = 0;
    char *tmp, *buffer;
    char c;

    /*Initialize buffer*/
    if(size == 0){
        buffer = malloc(sizeof(char));
        size = 1;
    }

    /*Read until next line or end of file*/
    for (i = 0;(c=getc(file)) != '\n' && c != EOF; i++){
        if(i == size){
            tmp = realloc(buffer, (++size)*sizeof(char));
            if(tmp == NULL){
                printf("Error: Memory allocation failed.\n");
                free(tmp);
                free(buffer);
                return 0;
            }
            buffer = tmp;
        }
        buffer[i] = c;
    }

    /*Add null terminator*/
    if(i == size){
        tmp = realloc(buffer, (++size)*sizeof(char));
        if(tmp == NULL){
            printf("Error: Memory allocation failed\n");
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

    tokens->lineNumber = lineNumber;

    tokens->fields[0] = strdup(strtok(line, " \t"));
    while(i < NUMBER_OF_FIELDS && tokens->fields[i] != NULL) {
        i++;
        tokens->fields[i] = strdup(strtok(NULL, " \t"));
    }
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
    /*TODO: add support for more fields*/
    printf("Line number: %d\n", line->lineNumber);
    printf("1\t\t2\t\t3\t\t4\n");
    printf("%s\t\t%s\t\t%s\t\t%s\n", line->fields[0], line->fields[1], line->fields[2], line->fields[3]);
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