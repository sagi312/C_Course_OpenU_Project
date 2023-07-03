#include <stdio.h>
#include <string.h>
#include "table.h"

/*struct TokenLine
{
    int lineNumber;
    char* firstField;
    char* secondField;
    char* thirdField;
    char* forthField;
    char* extra;
};*/

struct MacroTable{
    char* macroName;
    char* macroData;
    struct MacroTable* nextMacro;
};

struct Table{
    char* cellName;
    void* cellData;
    struct Table* nextCell;
};


/*Return 1 if name is in given table, 0 if not.*/
int inTable(char* name, Table* table){
    Table* pointer = table;
    while(pointer->nextCell != NULL){
        if(strcmp(pointer->cellName, name)){
            return 1;
        }
    }
}

/*Adds entry to given table. Returns 1 if succeeded, 0 if not.*/
int addTableEntry(char* name, char* data, Table** table){
    
    /*To loop on table*/
    Table* pointer = *table;
    
    /*Create cell to add*/
    Table* toAdd = (Table*) malloc(sizeof(Table));
    if(!toAdd){
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 0;
    }
    toAdd->cellName = name;
    toAdd->cellData = data;
    toAdd->nextCell = NULL;

    /*If first entry in table:*/
    if(*table == NULL){
        *table = toAdd;
        return 1;
    }
    
    /*Loop until last cell*/
    while(pointer->nextCell != NULL){
        pointer = pointer->nextCell;
    }

    pointer->nextCell = toAdd;
    return 1;
}