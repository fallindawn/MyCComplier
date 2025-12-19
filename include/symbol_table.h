#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>
#include <string.h>
#include "ast.h"

#define MAX_SYMBOLS 1000
#define MAX_FUNCTIONS 100
#define MAX_PARAMS 20

/* 符号类型 */
typedef enum {
    SYM_VARIABLE,
    SYM_FUNCTION
} SymbolKind;

/* 函数参数信息 */
typedef struct {
    char name[256];
    DataType type;
} FunctionParam;

/* 符号表条目 */
typedef struct {
    char name[256];
    DataType type;
    SymbolKind kind;
    int scope;
    int line;
    /* 函数相关信息 */
    FunctionParam params[MAX_PARAMS];
    int paramCount;
    int isDefinition;  /* 1: 定义, 0: 声明 */
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
void insertFunction(SymbolTable *table, const char *name, DataType returnType, int line);
void addFunctionParam(SymbolTable *table, const char *funcName, const char *paramName, DataType paramType);
Symbol* lookupSymbol(SymbolTable *table, const char *name);
int isSymbolDefined(SymbolTable *table, const char *name);
int isFunctionDefined(SymbolTable *table, const char *name);
void printSymbolTable(SymbolTable *table);
void printSymbolTableToFile(SymbolTable *table, FILE *file);
void freeSymbolTable(SymbolTable *table);

#endif
