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

/*Perform the second pass on the file. Replace all label placeholders with label addresses and create entry and extern tables*/
int secondPass(FILE* file, Table* symbolTable, Table* fileTable, Table* externTable, Table* entryTable);
