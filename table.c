#include "types.h"
#include <stdio.h>
#include <string.h>

/*Return 1 if name is in given table, 0 if not.*/
int inTable(char* name, Table* table){
    Table* pointer = table;
    while(pointer->nextCell != NULL){
        if(strcmp(pointer->cellData, name)){
            return 1;
        }
    }
}

/*Adds entry to given table. Returns 1 if succeeded, 0 if not.*/
int addTableEntry(char* name, char* data, Table* table){
    Table* pointer = table;
    Table* toAdd = (Table*) malloc(sizeof(Table));
    if(!toAdd){
        perror("Error: Memory allocation failed.");
        return 0;
    }
    toAdd->cellName = name;
    toAdd->cellData = data;
    toAdd->nextCell = NULL;
    while(pointer->nextCell != NULL){
        pointer = pointer->nextCell;
    }
    pointer->nextCell = toAdd;
    return 1;
}