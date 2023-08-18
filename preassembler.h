#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "converter.h"
#include "stringUtils.h"
#include "table.h"
#include "inputOutput.h"

/*Preassemble the file to the file table by expending all macros*/
int preAssemble(FILE* file, Table* fileTable);