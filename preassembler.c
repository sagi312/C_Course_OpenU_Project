#include <stdio.h>
#include <string.h>
#include "table.h"

Table* instructionTable;
/*Input: file pointer
Output: create .am extended file*/
int preAssemble(FILE* file){
    addTableEntryInt("mov", 0, &instructionTable);
    addTableEntryInt("cmp", 1, &instructionTable);
    addTableEntryInt("add", 2, &instructionTable);
    addTableEntryInt("sub", 3, &instructionTable);
    addTableEntryInt("not", 4, &instructionTable);
    addTableEntryInt("clr", 5, &instructionTable);
    addTableEntryInt("lea", 6, &instructionTable);
    addTableEntryInt("inc", 7, &instructionTable);
    addTableEntryInt("dec", 8, &instructionTable);
    addTableEntryInt("jmp", 9, &instructionTable);
    addTableEntryInt("bne", 10, &instructionTable);
    addTableEntryInt("red", 11, &instructionTable);
    addTableEntryInt("prn", 12, &instructionTable);
    addTableEntryInt("jsr", 13, &instructionTable);
    addTableEntryInt("rts", 14, &instructionTable);
    addTableEntryInt("stop", 15, &instructionTable);

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
