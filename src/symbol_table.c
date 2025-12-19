#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

SymbolTable* createSymbolTable(void) {
    SymbolTable *table = (SymbolTable *)malloc(sizeof(SymbolTable));
    table->count = 0;
    table->scope = 0;
    return table;
}

void insertSymbol(SymbolTable *table, const char *name, DataType type, int line) {
    if (table == NULL || table->count >= MAX_SYMBOLS) return;
    
    /* 检查重复定义 */
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->symbols[i].name, name) == 0 && 
            table->symbols[i].kind == SYM_VARIABLE) {
            fprintf(stderr, "Error: Variable '%s' already defined at line %d\n", name, line);
            return;
        }
    }
    
    strcpy(table->symbols[table->count].name, name);
    table->symbols[table->count].type = type;
    table->symbols[table->count].kind = SYM_VARIABLE;
    table->symbols[table->count].scope = table->scope;
    table->symbols[table->count].line = line;
    table->symbols[table->count].paramCount = 0;
    table->symbols[table->count].isDefinition = 1;
    table->count++;
}

void insertFunction(SymbolTable *table, const char *name, DataType returnType, int line) {
    if (table == NULL || table->count >= MAX_SYMBOLS) return;
    
    /* 检查重复的函数定义 */
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->symbols[i].name, name) == 0 && 
            table->symbols[i].kind == SYM_FUNCTION && 
            table->symbols[i].isDefinition == 1) {
            fprintf(stderr, "Error: Function '%s' already defined at line %d\n", name, line);
            return;
        }
    }
    
    strcpy(table->symbols[table->count].name, name);
    table->symbols[table->count].type = returnType;
    table->symbols[table->count].kind = SYM_FUNCTION;
    table->symbols[table->count].scope = table->scope;
    table->symbols[table->count].line = line;
    table->symbols[table->count].paramCount = 0;
    table->symbols[table->count].isDefinition = 1;
    table->count++;
}

void addFunctionParam(SymbolTable *table, const char *funcName, const char *paramName, DataType paramType) {
    if (table == NULL) return;
    
    /* 查找最后定义的函数 */
    for (int i = table->count - 1; i >= 0; i--) {
        if (table->symbols[i].kind == SYM_FUNCTION && 
            strcmp(table->symbols[i].name, funcName) == 0) {
            
            if (table->symbols[i].paramCount < MAX_PARAMS) {
                strcpy(table->symbols[i].params[table->symbols[i].paramCount].name, paramName);
                table->symbols[i].params[table->symbols[i].paramCount].type = paramType;
                table->symbols[i].paramCount++;
            }
            return;
        }
    }
}

Symbol* lookupSymbol(SymbolTable *table, const char *name) {
    if (table == NULL) return NULL;
    
    for (int i = table->count - 1; i >= 0; i--) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return &table->symbols[i];
        }
    }
    return NULL;
}

int isSymbolDefined(SymbolTable *table, const char *name) {
    Symbol *sym = lookupSymbol(table, name);
    return sym != NULL && sym->kind == SYM_VARIABLE;
}

int isFunctionDefined(SymbolTable *table, const char *name) {
    Symbol *sym = lookupSymbol(table, name);
    return sym != NULL && sym->kind == SYM_FUNCTION;
}

void printSymbolTable(SymbolTable *table) {
    if (table == NULL) return;
    
    printf("\n========== Symbol Table ==========\n");
    printf("%-30s %-20s %-20s %-10s\n", "Name", "Kind", "Type", "Line");
    printf("-----------------------------------------------------------\n");
    
    for (int i = 0; i < table->count; i++) {
        const char *typeStr = "unknown";
        const char *kindStr = "variable";
        
        if (table->symbols[i].kind == SYM_FUNCTION) {
            kindStr = "function";
        }
        
        switch(table->symbols[i].type) {
            case TYPE_INT: typeStr = "int"; break;
            case TYPE_FLOAT: typeStr = "float"; break;
            case TYPE_DOUBLE: typeStr = "double"; break;
            case TYPE_CHAR: typeStr = "char"; break;
            case TYPE_SHORT: typeStr = "short"; break;
            case TYPE_LONG: typeStr = "long"; break;
            case TYPE_LONG_LONG: typeStr = "long long"; break;
            case TYPE_UNSIGNED_INT: typeStr = "unsigned int"; break;
            case TYPE_UNSIGNED_FLOAT: typeStr = "unsigned float"; break;
            case TYPE_UNSIGNED_DOUBLE: typeStr = "unsigned double"; break;
            case TYPE_UNSIGNED_CHAR: typeStr = "unsigned char"; break;
            case TYPE_UNSIGNED_SHORT: typeStr = "unsigned short"; break;
            case TYPE_UNSIGNED_LONG: typeStr = "unsigned long"; break;
            case TYPE_UNSIGNED_LONG_LONG: typeStr = "unsigned long long"; break;
            case TYPE_BOOL: typeStr = "bool"; break;
            case TYPE_VOID: typeStr = "void"; break;
            case TYPE_UNKNOWN: typeStr = "unknown"; break;
        }
        
        printf("%-30s %-20s %-20s %-10d\n", table->symbols[i].name, kindStr, typeStr, table->symbols[i].line);
        
        /* 打印函数参数 */
        if (table->symbols[i].kind == SYM_FUNCTION && table->symbols[i].paramCount > 0) {
            printf("    Parameters: ");
            for (int j = 0; j < table->symbols[i].paramCount; j++) {
                if (j > 0) printf(", ");
                printf("%s: ", table->symbols[i].params[j].name);
                
                const char *paramTypeStr = "unknown";
                switch(table->symbols[i].params[j].type) {
                    case TYPE_INT: paramTypeStr = "int"; break;
                    case TYPE_FLOAT: paramTypeStr = "float"; break;
                    case TYPE_DOUBLE: paramTypeStr = "double"; break;
                    case TYPE_CHAR: paramTypeStr = "char"; break;
                    case TYPE_VOID: paramTypeStr = "void"; break;
                    default: break;
                }
                printf("%s", paramTypeStr);
            }
            printf("\n");
        }
    }
    printf("===================================\n\n");
}

void printSymbolTableToFile(SymbolTable *table, FILE *file) {
    if (table == NULL || file == NULL) return;
    
    fprintf(file, "\n========== Symbol Table ==========\n");
    fprintf(file, "%-30s %-20s %-20s %-10s\n", "Name", "Kind", "Type", "Line");
    fprintf(file, "-----------------------------------------------------------\n");
    
    for (int i = 0; i < table->count; i++) {
        const char *typeStr = "unknown";
        const char *kindStr = "variable";
        
        if (table->symbols[i].kind == SYM_FUNCTION) {
            kindStr = "function";
        }
        
        switch(table->symbols[i].type) {
            case TYPE_INT: typeStr = "int"; break;
            case TYPE_FLOAT: typeStr = "float"; break;
            case TYPE_DOUBLE: typeStr = "double"; break;
            case TYPE_CHAR: typeStr = "char"; break;
            case TYPE_SHORT: typeStr = "short"; break;
            case TYPE_LONG: typeStr = "long"; break;
            case TYPE_LONG_LONG: typeStr = "long long"; break;
            case TYPE_UNSIGNED_INT: typeStr = "unsigned int"; break;
            case TYPE_UNSIGNED_FLOAT: typeStr = "unsigned float"; break;
            case TYPE_UNSIGNED_DOUBLE: typeStr = "unsigned double"; break;
            case TYPE_UNSIGNED_CHAR: typeStr = "unsigned char"; break;
            case TYPE_UNSIGNED_SHORT: typeStr = "unsigned short"; break;
            case TYPE_UNSIGNED_LONG: typeStr = "unsigned long"; break;
            case TYPE_UNSIGNED_LONG_LONG: typeStr = "unsigned long long"; break;
            case TYPE_BOOL: typeStr = "bool"; break;
            case TYPE_VOID: typeStr = "void"; break;
            case TYPE_UNKNOWN: typeStr = "unknown"; break;
        }
        
        fprintf(file, "%-30s %-20s %-20s %-10d\n", table->symbols[i].name, kindStr, typeStr, table->symbols[i].line);
        
        /* 打印函数参数 */
        if (table->symbols[i].kind == SYM_FUNCTION && table->symbols[i].paramCount > 0) {
            fprintf(file, "    Parameters: ");
            for (int j = 0; j < table->symbols[i].paramCount; j++) {
                if (j > 0) fprintf(file, ", ");
                fprintf(file, "%s: ", table->symbols[i].params[j].name);
                
                const char *paramTypeStr = "unknown";
                switch(table->symbols[i].params[j].type) {
                    case TYPE_INT: paramTypeStr = "int"; break;
                    case TYPE_FLOAT: paramTypeStr = "float"; break;
                    case TYPE_DOUBLE: paramTypeStr = "double"; break;
                    case TYPE_CHAR: paramTypeStr = "char"; break;
                    case TYPE_VOID: paramTypeStr = "void"; break;
                    default: break;
                }
                fprintf(file, "%s", paramTypeStr);
            }
            fprintf(file, "\n");
        }
    }
    fprintf(file, "===================================\n\n");
}

void freeSymbolTable(SymbolTable *table) {
    if (table != NULL) {
        free(table);
    }
}

