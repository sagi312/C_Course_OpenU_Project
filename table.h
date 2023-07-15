#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef TABLE_H
#define TABLE_H
typedef struct Table Table;
#endif

/*Table operations*/
Table* createTable(void);
int addCell(char* name, Table* table);
int setCellData(char* name, char* data, Table* table);
char* getCellData(char* name, Table* table);
int removeCell(char* name, Table* table);
int inTable(char* name, Table* table);
int getTableSize(Table* table);
int freeTable(Table* table);
int printTable(Table* table);

char* strdup(char* str); /*TODO: move to other file. Helper functions?*/