typedef struct TokenLine
{
    int lineNumber;
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

typedef struct Table{
    char* cellName;
    char* cellData;
    Table* nextCell;
} Table;