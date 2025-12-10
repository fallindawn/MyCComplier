#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symbol_table.h"
#include "codegen.h"

extern ASTNode *root;
extern SymbolTable *symTable;
extern CodeGenerator *codeGen;
extern int yyparse();
extern void printTokens();
extern FILE *yyin;

int main(int argc, char *argv[]) {
    FILE *input_file = NULL;
    
    printf("==================================================\n");
    printf("          C Language Compiler v1.0\n");
    printf("==================================================\n\n");
    
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file.c>\n", argv[0]);
        return 1;
    }
    
    /* 打开输入文件 */
    input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", argv[1]);
        return 1;
    }
    
    /* 初始化 */
    symTable = createSymbolTable();
    codeGen = createCodeGenerator();
    yyin = input_file;
    
    printf("Input File: %s\n\n", argv[1]);
    
    /* 词法分析和语法分析 */
    printf("Starting lexical and syntax analysis...\n\n");
    int result = yyparse();
    
    if (result == 0) {
        printf("Syntax analysis completed successfully!\n\n");
        
        /* 打印词法分析结果 */
        printTokens();
        
        /* 打印语法树 */
        if (root != NULL) {
            printf("\n========== Abstract Syntax Tree (AST) ==========\n");
            printAST(root, 0);
            printf("================================================\n\n");
        }
        
        /* 打印符号表 */
        printSymbolTable(symTable);
        
        /* 生成中间代码 */
        printf("Generating intermediate code...\n");
        generateIntermediateCode(root, codeGen, symTable);
        printIntermediateCode(codeGen);
        
        printf("Compilation completed successfully!\n");
    } else {
        fprintf(stderr, "Compilation failed!\n");
    }
    
    /* 清理 */
    if (root != NULL) {
        freeAST(root);
    }
    if (symTable != NULL) {
        freeSymbolTable(symTable);
    }
    if (codeGen != NULL) {
        freeCodeGenerator(codeGen);
    }
    if (input_file != NULL) {
        fclose(input_file);
    }
    
    return result;
}
