#include <stdio.h>
#include <math.h>
#include "config.h"
#include "table.h"

#ifndef INPUTOUTPUT_H
#define INPUTOUTPUT_H
typedef struct TokenLine TokenLine;
#endif

/*Read a line dynammicly from file*/
char* readLine(FILE* file);

/*Tokenize a given line*/
TokenLine* tokenizeLine(char* line, int lineNumber);
/*Free a token line*/
int freeTokenLine(TokenLine* line);
/*Print a token line*/
int printTokenLine(TokenLine* line);
/*Get the n'th (starting from 0) token field from a token line*/
char* getTokenField(int num, TokenLine* line);
/*Get the line number from a token field*/
int getLineNumber(TokenLine* line);

/*Write all the cells' data in the table to a file*/
int writeFileFromTableData(FILE* file, Table* table);

/*Strip a string from spaces*/
char* strip(char*);
/*Duplicate a string. My version because the original isn't in the ansi standard*/
char* strdup(char* str);

/*Print a warning*/
void printWarning(char* warning, int lineNumber);
/*Print an error*/
void printError(char* error, int lineNumber);
/*Check if there was any errors printed*/
int hasErrors(void);
/*Reset the error flag*/
int resetErrors(void);