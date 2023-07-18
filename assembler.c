#include <stdio.h>
#include "table.h"
#include "inputOutput.h"


int testTable(void);
int main(int argc, char* argv[]){
    testInput();
    return 0;
    /*return preAssemble(NULL);*/
}

int testInput(void){
    FILE* file = openFile("test.txt");
    char* line, *line2;
    TokenLine* tokenLine;

    line = readLine(file);
    printf("Read: %s\n", line);
    line2 = readLine(file);
    printf("Read: %s\n", line2);
    tokenLine = tokenizeLine(line, 1);
    printTokenLine(tokenLine);
    printf("Line number: %d\n", getLineNumber(tokenLine));
    printf("Field 1: %s\n", getTokenField(0, tokenLine));
    printf("Field 2: %s\n", getTokenField(1, tokenLine));
    printf("Field 3: %s\n", getTokenField(2, tokenLine));
    printf("Field 4: %s\n", getTokenField(3, tokenLine));
    printf("Extra: %s\n", getTokenField(4, tokenLine));
    
    freeTokenLine(tokenLine);
    closeFile(file);
    free(line);
    free(line2);
    printTokenLine(tokenLine);
    printf("Done.\n");

    return 1;
}