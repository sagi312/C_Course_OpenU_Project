#include <stdio.h>
#include <string.h>
#include "table.h"
#include "inputOutput.h"
#include "config.h"

/*Preassemble the file to the file table by expending all macros*/
int preAssemble(FILE* file, Table* fileTable);