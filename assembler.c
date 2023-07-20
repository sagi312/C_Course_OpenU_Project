#include <stdio.h>
#include "table.h"
#include "inputOutput.h"
#include "preassembler.h"


int testPreAssemble(void);
int testInput(void);

int main(int argc, char* argv[]){
    testPreAssemble();
    return 0;
    /*return preAssemble(NULL);*/
}  

int testInput(void){
    FILE* file = fopen("test.txt", "r");
    char* line;
    TokenLine* tokenize;
    line = readLine(file);
    while(strlen(line) > 0 && line != NULL){
        printf("%s\n", line);
        tokenize = tokenizeLine(line, 0);
        printTokenLine(tokenize);
        freeTokenLine(tokenize);
        free(line);
        line = readLine(file);
    }

    return 0;
}

int testPreAssemble(void){
    FILE* file = fopen("test.txt", "r");
    preAssemble(file);
    return 0;
}