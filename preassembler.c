#include <stdio.h>
#include <string.h>
#include "table.h"

Table* instructionTable;
/*Input: file pointer
Output: create .am extended file*/
int preAssemble(FILE* file){
    addTableEntry("mov", 0, &instructionTable);
    addTableEntry("cmp", 1, &instructionTable);
    addTableEntry("add", 2, &instructionTable);
    addTableEntry("sub", 3, &instructionTable);
    addTableEntry("not", 4, &instructionTable);
    addTableEntry("clr", 5, &instructionTable);
    addTableEntry("lea", 6, &instructionTable);
    addTableEntry("inc", 7, &instructionTable);
    addTableEntry("dec", 8, &instructionTable);
    addTableEntry("jmp", 9, &instructionTable);
    addTableEntry("bne", 10, &instructionTable);
    addTableEntry("red", 11, &instructionTable);
    addTableEntry("prn", 12, &instructionTable);
    addTableEntry("jsr", 13, &instructionTable);
    addTableEntry("rts", 14, &instructionTable);
    addTableEntry("stop", 15, &instructionTable);

    return 0;
}

/*Input: tokenLine
Output: 1 if line is mcro start and valid, 0 if not*/
int checkMacro(TokenLine* line, int inMacro){
    if(inMacro){
        fprintf(stderr, "Error: nested macros aren't allowed.\n");
        return 0;
    }
    if(!strcmp(line->firstField, "mcro")){
        if(inTable(line->secondField, instructionTable)){
            fprintf(stderr, "Error: macro name isn't valid, line %d.\n", line->lineNumber);
            return 0;
        }
        else{
            addMacroTableEntry(line->secondField);
            return 1;
        }
    }
    else{ /*Not a macro*/
        return 0;
    }
}

/*Input: tokenLine
Output: 1 if line is mcro end and valid, 0 if not*/
int checkEndMacro(TokenLine* line, int inMcro){

}

/*Input: mcro name
Output: 1 if added to macro table, 0 if not
Action: adds new entry to the mcro table with the mcro name*/
int addMacroTableEntry(char* name){
    return 0;
}

/*Input: mcro name, line to add
Output: 1 if added to macro table, 0 if not
Action: adds the line to the last mcro table entry*/
int addMacroTableLine(char* line){

}

/*Input: file pointer
Output: 1 if replaced successfully, 0 if not
Action: replace last line with the mcro from the mcro table*/
int replaceMacro(FILE* file, char* name){

}

/*Input: tokenLine
Output: 1 if line is mcro name in table, 0 if not*/
int inMacroTable(TokenLine* line){

}
