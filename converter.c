#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "inputOutput.h"
#include "converter.h"


/*Convert a 12 bits binary word to base64*/
char* base64Encode(char* binary){
    /*split into 2 groups of 6 bits, convery to integer and translate to corrospounding char*/
    char* group1, *group2, *res;
    int i, dec;

    if(binary == NULL || strlen(binary) != 12)
        return NULL;

    group1 = malloc(7*sizeof(char));
    group2 = malloc(7*sizeof(char));
    res = malloc(3*sizeof(char));

    if(group1 == NULL || group2 == NULL || res == NULL){
        printError("Memory allocation failed", -1);
        if(group1 != NULL)
            free(group1);
        if(group2 != NULL)
            free(group2);
        if(res != NULL)
            free(res);
        return NULL;
    }

    for(i = 0; i < 6; i++){
        group1[i] = binary[i];
        group2[i] = binary[i+6];
    }

    group1[6] = '\0';
    group2[6] = '\0';

    dec = btoiUnsigned(group1);
    res[0] = itob64(dec);
    dec = btoiUnsigned(group2);
    res[1] = itob64(dec);
    res[2] = '\0';

    free(group1);
    free(group2);
    return res;
}

/*convert an int to base 64 char*/
char itob64(int dec){
    if(dec >= 0 && dec <= 25)
        return (char)(dec + 'A');
    else if(dec >= 26 && dec <= 51)
        return (char)(dec - 26 + 'a');
    else if(dec >= 52 && dec <= 61)
        return (char)(dec - 52 + '0');
    else if(dec == 62)
        return '+';
    else if(dec == 63)
        return '/';
    else
        return '\0';
}

/*convert a binary word to a positive int*/
int btoiUnsigned(char* binary){
    int res = 0, i = 0, length = 0;
    if(binary == NULL || strlen(binary) < 1)
        return 0;

    length = strlen(binary);
    for(i = 0; i < length; i++){
        if(binary[length-1-i] == '1')
            res += pow(2, i);
    }
    return res;
}

/*convert a binary word to an int by using 2's complement*/
int btoi(char* binary){
    int res = 0, i = 0, length = 0, sign = 1;
    if(binary == NULL || strlen(binary) < 1)
        return 0;

    length = strlen(binary);
    /*If negetive, we need to flip all bits, add 1, convert to decimal and convert to negetive*/
    if(binary[0] == '1'){
        sign = -1;
        for(i = 0; i < length; i++){
            if(binary[i] == '1')
                binary[i] = '0';
            else
                binary[i] = '1';
        }
        /*Add 1*/
        i = 0;
        while(i < length && binary[length-1-i] == '1'){
            binary[length-1-i] = '0';
            i++;
        }
        if(i < 12)
            binary[length-1-i] = '1';
        else
            binary[length-1-i] = '0';
    }

    for(i = 0; i < length; i++){
        if(binary[length-1-i] == '1')
            res += pow(2, i);
    }

    return res * sign;
}

/*convert an int to a 12 bits binary word by using 2's complement*/
char* itob(int num){
    char *res = malloc((BINARY_WORD_SIZE+1)*sizeof(char));
    int i = 0;

    if(num < 0) {
        /*Invert the number*/
        num = num * -1;

        for(i = 0; i < BINARY_WORD_SIZE; i++){
            /*11-i because we're starting from the least segnificent digit, and we want to print the inverted number*/
            if((num & 1) == 1)
                res[BINARY_WORD_SIZE-1-i] = '0';
            else
                res[BINARY_WORD_SIZE-1-i] = '1';
            num >>= 1;
        }
    
        /*Add 1 to the number*/
        i = 0;
        while(i < BINARY_WORD_SIZE && res[BINARY_WORD_SIZE-1-i] == '1'){
            res[BINARY_WORD_SIZE-1-i] = '0';
            i++;
        }
        if(i < 12)
            res[BINARY_WORD_SIZE-1-i] = '1';
        else
            res[BINARY_WORD_SIZE-1-i] = '0';
    }
    else {
        for(i = 0; i < BINARY_WORD_SIZE; i++){
            /*11-i because we're starting from the least segnificent digit*/
            if((num & 1) == 1)
                res[BINARY_WORD_SIZE-1-i] = '1';
            else
                res[BINARY_WORD_SIZE-1-i] = '0';
            num >>= 1;
        }
    }

    res[BINARY_WORD_SIZE] = '\0';
    return res;
}

/*convert an int to a string representation*/
char* itoa(int num){
    char* res;
    int size = 0;
    if(num < 0)
        size = NUMBER_OF_DIGITS(num * -1) + 2;
    else if(num != 0)
        size = NUMBER_OF_DIGITS(num) + 1;
    res = malloc(sizeof(char) * size);
    if(res == NULL) {
        printError("Memory allocation failed", -1);
        return NULL;
    }
    sprintf(res, "%d", num);
    return res;
}