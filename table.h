#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef TABLE_H
#define TABLE_H
typedef struct Table Table;
#endif

/*Table operations*/
Table* createTable(void);                               /*Creates a table*/
int addCell(char* name, Table* table);                  /*Adds a cell to the table*/
int setCellData(char* name, char* data, Table* table);  /*Sets the data of a cell*/
char* getCellData(char* name, Table* table);            /*Gets the data of a cell*/
char* getCellName(int index, Table* table);             /*Gets the name of a cell*/
int removeCell(char* name, Table* table);               /*Removes a cell from the table*/
int inTable(char* name, Table* table);                  /*Checks if a cell is in the table*/
int getTableSize(Table* table);                         /*Gets the size of the table*/
int freeTable(Table* table);                            /*Frees the table*/
int printTable(Table* table);                           /*Prints the table*/

char* strdup(char* str); /*TODO: move to other file. Helper functions?*/