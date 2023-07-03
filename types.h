typedef struct TokenLine
{
    char* firstField;
    char* secondField;
    char* thirdField;
    char* forthField;
    char* extra;
} TokenLine;

typedef struct MacroTable{
    char* macroName;
    char* macroData;
    MacroTable* nextMacro;
} MacroTable;