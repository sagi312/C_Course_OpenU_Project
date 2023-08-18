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

char* base64Encode(char* binary){
    /*split into 2 groups of 6 bits, convery to integer and translate to corrospounding char*/
    char* group1, *group2, *res;
    int i, dec;

    if(binary == NULL || strlen(binary) != 12)
        return NULL;

    group1 = malloc(7*sizeof(char));
    group2 = malloc(7*sizeof(char));
    res = malloc(3*sizeof(char));

    if(group1 == NULL || group2 == NULL || res == NULL){
        printError("Memory allocation failed", -1);
        if(group1 != NULL)
            free(group1);
        if(group2 != NULL)
            free(group2);
        if(res != NULL)
            free(res);
        return NULL;
    }

    for(i = 0; i < 6; i++){
        group1[i] = binary[i];
        group2[i] = binary[i+6];
    }

    group1[6] = '\0';
    group2[6] = '\0';

    dec = btoiUnsigned(group1);
    res[0] = itob64(dec);
    dec = btoiUnsigned(group2);
    res[1] = itob64(dec);
    res[2] = '\0';

    if(!strcmp("101001110100", binary))
        printf("group1: %s, group2: %s, res: %s\n", group1, group2, res);
    free(group1);
    free(group2);
    return res;
}

char itob64(int dec){
    char res;
    if(dec >= 0 && dec <= 25)
        return (char)(dec + 'A');
    else if(dec >= 26 && dec <= 51)
        return (char)(dec - 26 + 'a');
    else if(dec >= 52 && dec <= 61)
        return (char)(dec - 52 + '0');
    else if(dec == 62)
        return '+';
    else if(dec == 63)
        return '/';
    else
        return '\0';
}

int btoiUnsigned(char* binary){
    int res = 0, i = 0, length = 0;
    if(binary == NULL || strlen(binary) < 1)
        return 0;

    length = strlen(binary);
    for(i = 0; i < length; i++){
        if(binary[length-1-i] == '1')
            res += pow(2, i);
    }
    return res;
}

int btoi(char* binary){
    int res = 0, i = 0, length = 0, sign = 1;
    if(binary == NULL || strlen(binary) < 1)
        return 0;

    length = strlen(binary);
    /*If negetive, we need to flip all bits, add 1, convert to decimal and convert to negetive*/
    if(binary[0] == '1'){
        sign = -1;
        for(i = 0; i < length; i++){
            if(binary[i] == '1')
                binary[i] = '0';
            else
                binary[i] = '1';
        }
        /*Add 1*/
        i = 0;
        while(i < length && binary[length-1-i] == '1'){
            binary[length-1-i] = '0';
            i++;
        }
        if(i < 12)
            binary[length-1-i] = '1';
        else
            binary[length-1-i] = '0';
    }

    for(i = 0; i < length; i++){
        if(binary[length-1-i] == '1')
            res += pow(2, i);
    }

    return res * sign;
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