#ifndef CONFIG_DEFS

#define CONFIG_DEFS
#define MAX_LABEL_LENGTH 31
#define RESERVED_NAMES {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop", "mcro", "endmcro", ".data", ".entry", ".extern", ".string", "@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7",}
#define RESERVED_NAMES_COUNT 30
#define OP_NAMES {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"}
#define OP_NAMES_COUNT 16

#endif