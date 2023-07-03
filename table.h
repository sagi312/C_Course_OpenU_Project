#include <stdio.h>
#include <string.h>

typedef struct TokenLine TokenLine;
struct TokenLine
{
    int lineNumber;
    char* firstField;
    char* secondField;
    char* thirdField;
    char* forthField;
    char* extra;
};
typedef struct MacroTable MacroTable;
typedef struct Table Table;

/*Return 1 if name is in given table, 0 if not.*/
int inTable(char* name, Table* table);

/*Adds entry to given table. Returns 1 if succeeded, 0 if not.*/
int addTableEntry(char* name, char* data, Table** table);


