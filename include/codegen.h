#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "symbol_table.h"

/* 四元式结构 */
typedef struct {
    char op[32];
    char arg1[256];
    char arg2[256];
    char result[256];
} Quadruple;

/* 汇编代码行结构 */
typedef struct {
    char code[256];
} AsmLine;

/* 中间代码生成器 */
typedef struct {
    Quadruple *quads;
    int count;
    int capacity;
    int tempVarCount;
    int labelCount;
    /* 汇编代码相关字段 */
    AsmLine *asmLines;
    int asmCount;
    int asmCapacity;
} CodeGenerator;

/* 函数声明 */
CodeGenerator* createCodeGenerator(void);
void genCode(CodeGenerator *gen, const char *op, const char *arg1, 
             const char *arg2, const char *result);
char* getTempVar(CodeGenerator *gen);
char* getLabel(CodeGenerator *gen);
const char* getOpString(OpType op);
void generateIntermediateCode(ASTNode *ast, CodeGenerator *gen, SymbolTable *table);
void generateIntermediateCodeExpr(ASTNode *ast, CodeGenerator *gen, SymbolTable *table, char *result);
void printIntermediateCode(CodeGenerator *gen);
void printIntermediateCodeToFile(CodeGenerator *gen, FILE *file);
/* 汇编代码生成函数 */
void generateAssemblyCode(CodeGenerator *gen, SymbolTable *table);
void optimizeAssemblyCode(CodeGenerator *gen);
void printAssemblyCode(CodeGenerator *gen);
void printAssemblyCodeToFile(CodeGenerator *gen, FILE *file);
void freeCodeGenerator(CodeGenerator *gen);

#endif
