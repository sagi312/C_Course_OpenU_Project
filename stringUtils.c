#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stringUtils.h"

/*Strip a string from spaces*/
char* strip(char* line){
    int i = 0;
    char* res;

    while(line[i] == ' ' || line[i] == '\t'){
        i++;
    }
    res = strdup(line + i);
    return res;
}

/*Duplicate a string. My version because the original isn't in the ansi standard*/
char* strdup(char* str){
    char* des;
    if(str == NULL){
        return NULL;
    }
    des = (char*) malloc(strlen(str)+1);
    if(des == NULL){
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return NULL;
    }
    strcpy(des, str);
    return des;
}