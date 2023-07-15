#include <stdio.h>
#include "table.h"

int main(int argc, char* argv[]){
    testTable();
    return 0;
    /*return preAssemble(NULL);*/
}

int testTable(){
    Table* table = createTable();
    printf("Table created\n");
    addCell("test", table);
    printf("Entry added\n");
    addCell("test2", table);
    printf("Entry added\n");
    printTable(table);
    printf("Table printed\n");
    setCellData("test", "data", table);
    printf("Data set\n");
    setCellData("test2", "10", table);
    printf("Data set\n");
    printTable(table);
    printf("test in table %d\n", inTable("test", table));
    printf("non in table %d\n", inTable("non", table));
    printf("Table size %d\n", getTableSize(table));
    removeCell("test", table);
    printf("Entry removed\n");
    printTable(table);
    freeTable(table);
    printf("Table freed\n");
    printTable(table);
}