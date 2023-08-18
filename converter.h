#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

/*Convert a 12 bits binary word to base64*/
char* base64Encode(char* binary);

/*convert an int to base 64 char*/
char itob64(int dec);

/*convert a binary word to a positive int*/
int btoiUnsigned(char* binary);

/*convert a binary word to an int by using 2's complement*/
int btoi(char* binary);

/*convert an int to a 12 bits binary word by using 2's complement*/
char* itob(int num);

/*convert an int to a string representation*/
char* itoa(int num);