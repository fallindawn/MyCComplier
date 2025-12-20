#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symbol_table.h"
#include "codegen.h"
#include "lexer.h"

extern ASTNode *root;
extern SymbolTable *symTable;
extern CodeGenerator *codeGen;
extern int yyparse();
extern void printTokens();
extern FILE *yyin;

int main(int argc, char *argv[]) {
    FILE *input_file = NULL;
    FILE *output_file = NULL;
    char output_filename[512];
    char input_basename[256];
    
    printf("==================================================\n");
    printf("          C Language Compiler\n");
    printf("==================================================\n\n");
    
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file.c>\n", argv[0]);
        return 1;
    }
    
    /* 打开输入 */
    input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", argv[1]);
        return 1;
    }
    
    const char *basename_ptr = argv[1];
    for (int i = strlen(argv[1]) - 1; i >= 0; i--) {
        if (argv[1][i] == '\\' || argv[1][i] == '/') {
            basename_ptr = &argv[1][i + 1];
            break;
        }
    }
    
    strcpy(input_basename, basename_ptr);
    /* 移除扩展名 */
    char *dot_pos = strchr(input_basename, '.');
    if (dot_pos != NULL) {
        *dot_pos = '\0';
    }
    
    /* 路径 */
    snprintf(output_filename, sizeof(output_filename), "output/%s_output.txt", input_basename);
    
    /* 打开输出文件 */
    output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        /* 尝试创建 output 目录（如果不存在） */
        system("mkdir output 2>nul || exit /b 0");
        output_file = fopen(output_filename, "w");
        if (output_file == NULL) {
            fprintf(stderr, "Error: Cannot create output file '%s'\n", output_filename);
            fclose(input_file);
            return 1;
        }
    }
    
    /* 初始化 */
    symTable = createSymbolTable();
    codeGen = createCodeGenerator();
    yyin = input_file;
    
    printf("Input File: %s\n\n", argv[1]);
    fprintf(output_file, "==================================================\n");
    fprintf(output_file, "          C Language Compiler\n");
    fprintf(output_file, "==================================================\n\n");
    fprintf(output_file, "Input File: %s\n\n", argv[1]);
    
    /* 词法分析和语法分析 */
    printf("Starting lexical and syntax analysis...\n\n");
    fprintf(output_file, "Starting lexical and syntax analysis...\n\n");
    int result = yyparse();
    
    if (result == 0) {
        printf("Syntax analysis completed successfully!\n\n");
        fprintf(output_file, "Syntax analysis completed successfully!\n\n");
        
        /* 打印词法分析结果->文件（1） */
        fprintf(output_file, "========== Lexical Analysis Results ==========\n");
        fprintf(output_file, "%-5s %-20s %-20s %-10s\n", "No.", "Token Type", "Lexeme", "Line");
        fprintf(output_file, "----------------------------------------------\n");
        extern TokenRecord tokenRecords[];
        extern int recordCount;
        for (int i = 0; i < recordCount; i++) {
            fprintf(output_file, "%-5d %-20s %-20s %-10d\n", i+1, tokenRecords[i].token, 
                    tokenRecords[i].lexeme, tokenRecords[i].lineNum);
        }
        fprintf(output_file, "==============================================\n\n");
        
        /* 打印词法分析结果->控制台（2） */
        printTokens();
        
        /* 打印语法树 */
        if (root != NULL) {
            fprintf(output_file, "\n========== Abstract Syntax Tree (AST) ==========\n");
            fprintf(output_file, "PROGRAM\n");
            /* 递归打印 AST ->文件 */
            printASTToFile(root, 0, output_file);
            fprintf(output_file, "================================================\n\n");
            
            printf("\n========== Abstract Syntax Tree (AST) ==========\n");
            printAST(root, 0);
            printf("================================================\n\n");
        }
        
        /* 打印符号表（3） */
        printSymbolTableToFile(symTable, output_file);
        printSymbolTable(symTable);
        
        /* 生成中间代码（4） */
        printf("Generating intermediate code...\n");
        fprintf(output_file, "Generating intermediate code...\n");
        generateIntermediateCode(root, codeGen, symTable);
        printIntermediateCodeToFile(codeGen, output_file);
        printIntermediateCode(codeGen);
        
        /* 生成汇编代码（5）*/
        printf("Generating assembly code...\n");
        fprintf(output_file, "Generating assembly code...\n");
        generateAssemblyCode(codeGen, symTable);
        
        /* 优化汇编代码 */
        printf("Optimizing assembly code...\n");
        fprintf(output_file, "Optimizing assembly code...\n");
        optimizeAssemblyCode(codeGen);
        
        printAssemblyCodeToFile(codeGen, output_file);
        printAssemblyCode(codeGen);
        
        printf("Compilation completed successfully!\n");
        fprintf(output_file, "Compilation completed successfully!\n");
    } else {
        fprintf(stderr, "Compilation failed!\n");
        fprintf(output_file, "Compilation failed!\n");
    }
    
    /* clean */
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
    if (output_file != NULL) {
        fclose(output_file);
        printf("Output saved to: %s\n", output_filename);
    }
    
    return result;
}
