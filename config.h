#ifndef CONFIG_DEFS

#define CONFIG_DEFS
#define MAX_LABEL_LENGTH 31
#define RESERVED_NAMES {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop", "mcro", "endmcro", ".data", ".entry", ".extern", ".string", "@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7",}
#define RESERVED_NAMES_COUNT 30
#define OP_NAMES {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"}
#define OP_NAMES_COUNT 16

#define OP_GROUP1 {"mov", "cmp", "add", "sub", "lea"}
#define OP_GROUP1_COUNT 5
#define OP_GROUP2 {"not", "clr", "inc", "dec", "jmp", "bne", "red", "prn", "jsr"}
#define OP_GROUP2_COUNT 9
#define OP_GROUP3 {"rts", "stop"}
#define OP_GROUP3_COUNT 2

#endif