#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>
#include <string.h>
#include "ast.h"

#define MAX_SYMBOLS 1000

/* 符号表条目 */
typedef struct {
    char name[256];
    DataType type;
    int scope;
    int line;
} Symbol;

/* 符号表 */
typedef struct {
    Symbol symbols[MAX_SYMBOLS];
    int count;
    int scope;
} SymbolTable;

/* 函数声明 */
SymbolTable* createSymbolTable(void);
void insertSymbol(SymbolTable *table, const char *name, DataType type, int line);
Symbol* lookupSymbol(SymbolTable *table, const char *name);
int isSymbolDefined(SymbolTable *table, const char *name);
void printSymbolTable(SymbolTable *table);
void freeSymbolTable(SymbolTable *table);

#endif
