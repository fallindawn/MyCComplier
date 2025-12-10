#ifndef LEXER_H
#define LEXER_H

typedef struct {
    char token[256];
    char lexeme[256];
    int lineNum;
} TokenRecord;

extern TokenRecord tokenRecords[];
extern int recordCount;
extern int lineNum;
extern void printTokens();

#endif
