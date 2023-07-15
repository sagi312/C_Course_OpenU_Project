typedef struct TokenLine TokenLine;
struct TokenLine
{
    int lineNumber;
    char* firstField;
    char* secondField;
    char* thirdField;
    char* forthField;
    char* extra;
};