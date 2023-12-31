#include <stdio.h>
#include "config.h"
#include "table.h"
#include "inputOutput.h"
#include "encoder.h"
#include "converter.h"
#include "typeChecker.h"

/*This function will encode the file's content into the given fileTable. It will
  not encode the labels, and instead will save them in the symbolTable for secondPass.
  The function will also save the ic and dc counters to *icOut and *dcOut respectivly*/
int firstPass(FILE* file, Table* symbolTable, Table* fileTable, int* icOut, int* dcOut);