#include <stdio.h>
#include <string.h>
#include "table.h"

Table* invalidTable;
/*Input: file pointer
Output: create .am extended file*/
int preAssemble(FILE* file){
    addTableEntryInt("mov", 0, &invalidTable);
    addTableEntryInt("cmp", 0, &invalidTable);
    addTableEntryInt("add", 0, &invalidTable);
    addTableEntryInt("sub", 0, &invalidTable);
    addTableEntryInt("not", 0, &invalidTable);
    addTableEntryInt("clr", 0, &invalidTable);
    addTableEntryInt("lea", 0, &invalidTable);
    addTableEntryInt("inc", 0, &invalidTable);
    addTableEntryInt("dec", 0, &invalidTable);
    addTableEntryInt("jmp", 0, &invalidTable);
    addTableEntryInt("bne", 0, &invalidTable);
    addTableEntryInt("red", 0, &invalidTable);
    addTableEntryInt("prn", 0, &invalidTable);
    addTableEntryInt("jsr", 0, &invalidTable);
    addTableEntryInt("rts", 0, &invalidTable);
    addTableEntryInt("stop", 0, &invalidTable);
    addTableEntryInt("mcro", 0, &invalidTable);
    addTableEntryInt("endmcro", 0, &invalidTable);
    addTableEntryInt(".data", 0, &invalidTable);
    addTableEntryInt(".entry", 0, &invalidTable);
    addTableEntryInt(".extern", 0, &invalidTable);
    addTableEntryInt(".string", 0, &invalidTable);



    return 0;
}

/*Input: tokenLine
Output: 1 if line is mcro start and valid, 0 if not*/
int checkMacro(TokenLine* line, int inMacro){
    if(inMacro){
        fprintf(stderr, "Error: nested macros aren't allowed, line %d.\n", line->lineNumber);
        return 0;
    }
    if(!strcmp(line->firstField, "mcro")){
        if(inTable(line->secondField, invalidTable)){
            fprintf(stderr, "Error: macro name isn't valid, line %d.\n", line->lineNumber);
            return 0;
        }
        else if(line->thirdField != NULL || line->forthField != NULL || line->extra != NULL){
            fprintf(stderr, "Error: extra text after macro start, line %d.\n", line->lineNumber);
            return 0;
        }
        return 1;
    }
    else{ /*Not a macro*/
        return 0;
    }
}

/*Input: tokenLine
Output: 1 if line is mcro end and valid, 0 if not*/
int checkEndMacro(TokenLine* line, int inMcro){
    if(!inMcro){
        fprintf(stderr, "Error: macro end without start of macro, line %d.\n", line->lineNumber);
        return 0;
    }
    if(!strcmp(line->firstField, "endmcro")){
        if(line->secondField != NULL || line->thirdField != NULL || line->forthField != NULL || line->extra != NULL){
            fprintf(stderr, "Error: extra text after macro end, line %d.\n", line->lineNumber);
        }
        return 1;
    }
    else{ /*Not ending a macro*/
        return 0;
    }
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
