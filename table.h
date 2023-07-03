#include "types.h"
#include <stdio.h>
#include <string.h>

/*Return 1 if name is in given table, 0 if not.*/
int inTable(char* name, Table* table);

/*Adds entry to given table. Returns 1 if succeeded, 0 if not.*/
int addTableEntry(char* name, char* data, Table* table);