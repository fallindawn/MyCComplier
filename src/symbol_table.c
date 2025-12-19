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
        if (strcmp(table->symbols[i].name, name) == 0) {
            fprintf(stderr, "Error: Variable '%s' already defined at line %d\n", name, line);
            return;
        }
    }
    
    strcpy(table->symbols[table->count].name, name);
    table->symbols[table->count].type = type;
    table->symbols[table->count].scope = table->scope;
    table->symbols[table->count].line = line;
    table->count++;
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
    return lookupSymbol(table, name) != NULL;
}

void printSymbolTable(SymbolTable *table) {
    if (table == NULL) return;
    
    printf("\n========== Symbol Table ==========\n");
    printf("%-30s %-20s %-10s\n", "Name", "Type", "Line");
    printf("-------------------------------------------\n");
    
    for (int i = 0; i < table->count; i++) {
        const char *typeStr = "unknown";
        
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
        
        printf("%-30s %-20s %-10d\n", table->symbols[i].name, typeStr, table->symbols[i].line);
    }
    printf("===================================\n\n");
}

void printSymbolTableToFile(SymbolTable *table, FILE *file) {
    if (table == NULL || file == NULL) return;
    
    fprintf(file, "\n========== Symbol Table ==========\n");
    fprintf(file, "%-30s %-20s %-10s\n", "Name", "Type", "Line");
    fprintf(file, "-------------------------------------------\n");
    
    for (int i = 0; i < table->count; i++) {
        const char *typeStr = "unknown";
        
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
        
        fprintf(file, "%-30s %-20s %-10d\n", table->symbols[i].name, typeStr, table->symbols[i].line);
    }
    fprintf(file, "===================================\n\n");
}

void freeSymbolTable(SymbolTable *table) {
    if (table != NULL) {
        free(table);
    }
}
