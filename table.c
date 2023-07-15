#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "table.h"

struct Table{
    char* cellName;
    char* cellData;
    struct Table* nextCell;
};


Table* createTable(void) {
    Table* table = (Table*) malloc(sizeof(Table));
    
    if(!table){
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return NULL;
    }
    
    table->cellName = strdup("Header");
    table->cellData = NULL;
    table->nextCell = NULL;

    if(table->cellName == NULL){
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return NULL;
    }

    return table;
}

int addCell(char* name, Table* table) {
    /*To loop on table*/
    Table* pointer = table;
    Table* toAdd = (Table*) malloc(sizeof(Table));
    
    if(table == NULL){
        return 0;
    }
    
    /*Create cell to add*/
    if(!toAdd){
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 0;
    }
    toAdd->cellName = strdup(name);
    toAdd->cellData = NULL;
    toAdd->nextCell = NULL;
    
    if(table->cellName == NULL){
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 0;
    }

    /*Loop until last cell*/
    while(pointer->nextCell != NULL){
        pointer = pointer->nextCell;
    }

    pointer->nextCell = toAdd;
    return 1;
}

int setCellData(char* name, char* data, Table* table){
    /*To loop on table*/
    Table* pointer = table;
    
    if(table == NULL){
        return 0;
    }
    
    /*Loop until last cell or found entry*/
    while(pointer != NULL){
        if(!strcmp(pointer->cellName, name)){
            pointer->cellData = strdup(data);
            if(pointer->cellData == NULL){
                fprintf(stderr, "Error: Memory allocation failed.\n");
                return 0;
            }
            return 1;
        }
        pointer = pointer->nextCell;
    }

    fprintf(stderr, "Error: Entry not found.\n");
    return 1;

}

char* getCellData(char* name, Table* table) {
    /*Return copy of string for saftey and enacpsulation*/
    static char* res = NULL;
    /*To loop on table*/
    Table* pointer = table;

    if(table == NULL){
        return NULL;
    }
    
    
    
    /*Loop until last cell or found entry*/
    while(pointer != NULL){
        if(!strcmp(pointer->cellName, name)){
            return res = strdup(pointer->cellData);
        }
        pointer = pointer->nextCell;
    }

    fprintf(stderr, "Error: Entry not found.\n");
    return NULL;
}

int removeCell(char* name, Table* table){
    /*To loop on table*/
    Table* pointer = table;
    
    if(table == NULL){
        return 0;
    }

    /*Loop until second last cell or found entry*/
    while(pointer->nextCell->nextCell != NULL){
        if(!strcmp(pointer->nextCell->cellName, name)){
            free(pointer->nextCell->cellName);
            free(pointer->nextCell->cellData);
            free(pointer->nextCell);
            pointer->nextCell = pointer->nextCell->nextCell;
            return 1;
        }
        pointer = pointer->nextCell;
    }
    /*Check last cell*/
    if(!strcmp(pointer->nextCell->cellName, name)){
            free(pointer->nextCell->cellName);
            free(pointer->nextCell->cellData);
            free(pointer->nextCell);
            pointer->nextCell = NULL;
            return 1;
    }

    fprintf(stderr, "Error: Entry not found.\n");
    return 0;
}

int inTable(char* name, Table* table) {
    /*To loop on table*/
    Table* pointer = table;

    if(table == NULL){
        return 0;
    }
    
    /*Loop until last cell or found entry*/
    while(pointer != NULL){
        if(!strcmp(pointer->cellName, name)){
            return 1;
        }
        pointer = pointer->nextCell;
    }

    return 0;
}

int getTableSize(Table* table) {
    /*To loop on table*/
    Table* pointer = table;
    int size = 0;

    if(table == NULL){
        return 0;
    }
    
    /*Loop until last cell*/
    while(pointer != NULL){
        size++;
        pointer = pointer->nextCell;
    }

    /*Subtract header*/
    return size-1;
}

int freeTable(Table* table) {
    Table* thisCell = table;
    Table* lastCell = table;

    if(table == NULL){
        return 0;
    }

    while(thisCell != NULL){
        lastCell = thisCell;
        thisCell = thisCell->nextCell;
        free(lastCell->cellName);
        free(lastCell->cellData);
        free(lastCell);
    }

    return 1;
}

int printTable(Table* table) {
    /*Skip header*/
    Table* pointer = table->nextCell;

    if(table == NULL){
        return 0;
    }

    /*TODO: print pretty if there are linebreaks*/
    printf("\nName\tData\n");
    while(pointer != NULL){
        if(pointer->cellData != NULL)
            printf("%s:\t%s\n", pointer->cellName, pointer->cellData);
        else
            printf("%s:\n", pointer->cellName);
        pointer = pointer->nextCell;
    }
    printf("\n");
    return 1;
}

char* strdup(char* str){
    char* des = (char*) malloc(strlen(str)+1);
    if(des == NULL){
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return NULL;
    }
    strcpy(des, str);
    return des;
}