#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "firstPass.h"
#include "table.h"
#include "inputOutput.h"
#include "config.h"
#include "typeChecker.h"

int secondPass(FILE* file, Table* symbolTable, Table* fileTable, Table* externTable, Table* entryTable);
 
int replaceLabels(Table* symbolTable, Table* fileTable, Table* externTable);

int addExternLabel(Table* symbolTable, Table* externTable);

int setEntryTable(FILE* file, Table* symbolTable, Table* entryTable);