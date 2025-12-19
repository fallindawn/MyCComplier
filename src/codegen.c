#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "codegen.h"

CodeGenerator* createCodeGenerator(void) {
    CodeGenerator *gen = (CodeGenerator *)malloc(sizeof(CodeGenerator));
    gen->capacity = 1000;
    gen->quads = (Quadruple *)malloc(gen->capacity * sizeof(Quadruple));
    gen->count = 0;
    gen->tempVarCount = 0;
    gen->labelCount = 0;
    gen->asmLines = NULL;
    gen->asmCount = 0;
    gen->asmCapacity = 0;
    return gen;
}

void genCode(CodeGenerator *gen, const char *op, const char *arg1, 
             const char *arg2, const char *result) {
    if (gen == NULL || gen->count >= gen->capacity) return;
    
    strncpy(gen->quads[gen->count].op, op ? op : "", 31);
    strncpy(gen->quads[gen->count].arg1, arg1 ? arg1 : "", 255);
    strncpy(gen->quads[gen->count].arg2, arg2 ? arg2 : "", 255);
    strncpy(gen->quads[gen->count].result, result ? result : "", 255);
    
    gen->quads[gen->count].op[31] = '\0';
    gen->quads[gen->count].arg1[255] = '\0';
    gen->quads[gen->count].arg2[255] = '\0';
    gen->quads[gen->count].result[255] = '\0';
    
    gen->count++;
}

char* getTempVar(CodeGenerator *gen) {
    static char buf[32];
    sprintf(buf, "t%d", gen->tempVarCount++);
    return buf;
}

char* getLabel(CodeGenerator *gen) {
    static char buf[32];
    sprintf(buf, "L%d", gen->labelCount++);
    return buf;
}

const char* getOpString(OpType op) {
    switch (op) {
        case OP_PLUS: return "+";
        case OP_MINUS: return "-";
        case OP_MULT: return "*";
        case OP_DIV: return "/";
        case OP_MOD: return "%";
        case OP_POW: return "^";
        case OP_EQ: return "==";
        case OP_NE: return "!=";
        case OP_LT: return "<";
        case OP_GT: return ">";
        case OP_LE: return "<=";
        case OP_GE: return ">=";
        case OP_AND: return "&&";
        case OP_OR: return "||";
        case OP_NOT: return "!";
        default: return "?";
    }
}

void generateIntermediateCode(ASTNode *ast, CodeGenerator *gen, SymbolTable *table) {
    if (ast == NULL || gen == NULL) return;
    
    static char buf[256];
    
    switch (ast->type) {
        case NODE_PROGRAM:
        case NODE_DECL_LIST:
        case NODE_STMT_LIST:
            for (int i = 0; i < ast->childrenCount; i++) {
                generateIntermediateCode(ast->children[i], gen, table);
            }
            break;
            
        case NODE_DECL:
            if (ast->childrenCount > 0) {
                ASTNode *idNode = ast->children[0];
                if (idNode->type == NODE_ID) {
                    sprintf(buf, "var %s", idNode->name);
                    genCode(gen, "DECL", idNode->name, "", "");
                }
            }
            break;
            
        case NODE_ASSIGN:
            if (ast->childrenCount >= 2) {
                char *result = ast->children[0]->name;
                char *tempVar = getTempVar(gen);
                generateIntermediateCodeExpr(ast->children[1], gen, table, tempVar);
                genCode(gen, "=", tempVar, "", result);
            }
            break;
            
        case NODE_WRITE:
            if (ast->childrenCount > 0) {
                char *tempVar = getTempVar(gen);
                generateIntermediateCodeExpr(ast->children[0], gen, table, tempVar);
                genCode(gen, "WRITE", tempVar, "", "");
            }
            break;
            
        case NODE_READ:
            if (ast->childrenCount > 0) {
                genCode(gen, "READ", ast->children[0]->name, "", "");
            }
            break;
            
        default:
            for (int i = 0; i < ast->childrenCount; i++) {
                generateIntermediateCode(ast->children[i], gen, table);
            }
            break;
    }
}

void generateIntermediateCodeExpr(ASTNode *ast, CodeGenerator *gen, SymbolTable *table, char *result) {
    if (ast == NULL || gen == NULL) return;
    
    char buf[256];
    char *temp1, *temp2;
    
    switch (ast->type) {
        case NODE_NUM:
            sprintf(buf, "%d", ast->intValue);
            genCode(gen, "=", buf, "", result);
            break;
            
        case NODE_ID:
            genCode(gen, "=", ast->name, "", result);
            break;
            
        case NODE_BINOP: {
            if (ast->childrenCount >= 2) {
                temp1 = getTempVar(gen);
                temp2 = getTempVar(gen);
                generateIntermediateCodeExpr(ast->children[0], gen, table, temp1);
                generateIntermediateCodeExpr(ast->children[1], gen, table, temp2);
                genCode(gen, getOpString(ast->op), temp1, temp2, result);
            }
            break;
        }
            
        case NODE_UNOP:
            if (ast->childrenCount > 0) {
                temp1 = getTempVar(gen);
                generateIntermediateCodeExpr(ast->children[0], gen, table, temp1);
                genCode(gen, getOpString(ast->op), temp1, "", result);
            }
            break;
            
        case NODE_CALL: {
            /* 函数调用 */
            if (ast->childrenCount > 0) {
                ASTNode *funcName = ast->children[0];
                char funcCall[256];
                
                if (funcName->type == NODE_ID) {
                    /* 生成函数调用四元式 */
                    if (ast->childrenCount > 1 && ast->children[1]->type == NODE_ARG_LIST) {
                        /* 有参数的函数调用 */
                        ASTNode *argList = ast->children[1];
                        
                        /* 为每个参数生成代码 */
                        for (int i = 0; i < argList->childrenCount; i++) {
                            char *argTemp = getTempVar(gen);
                            generateIntermediateCodeExpr(argList->children[i], gen, table, argTemp);
                            /* PARAM 操作符，参数在 arg1 */
                            genCode(gen, "PARAM", argTemp, "", "");
                        }
                    }
                    
                    /* 生成函数调用指令 */
                    sprintf(funcCall, "CALL %s", funcName->name);
                    genCode(gen, funcCall, "", "", result);
                }
            }
            break;
        }
            
        default:
            break;
    }
}

void printIntermediateCode(CodeGenerator *gen) {
    if (gen == NULL) return;
    
    printf("\n========== Intermediate Code (Quadruples) ==========\n");
    printf("%-5s %-15s %-20s %-20s %-20s\n", "Num", "Op", "Arg1", "Arg2", "Result");
    printf("-----------------------------------------------------\n");
    
    for (int i = 0; i < gen->count; i++) {
        printf("%-5d %-15s %-20s %-20s %-20s\n", 
               i, 
               gen->quads[i].op,
               gen->quads[i].arg1,
               gen->quads[i].arg2,
               gen->quads[i].result);
    }
    printf("====================================================\n\n");
}

void printIntermediateCodeToFile(CodeGenerator *gen, FILE *file) {
    if (gen == NULL || file == NULL) return;
    
    fprintf(file, "\n========== Intermediate Code (Quadruples) ==========\n");
    fprintf(file, "%-5s %-15s %-20s %-20s %-20s\n", "Num", "Op", "Arg1", "Arg2", "Result");
    fprintf(file, "-----------------------------------------------------\n");
    
    for (int i = 0; i < gen->count; i++) {
        fprintf(file, "%-5d %-15s %-20s %-20s %-20s\n", 
               i, 
               gen->quads[i].op,
               gen->quads[i].arg1,
               gen->quads[i].arg2,
               gen->quads[i].result);
    }
    fprintf(file, "====================================================\n\n");
}

void freeCodeGenerator(CodeGenerator *gen) {
    if (gen != NULL) {
        if (gen->quads != NULL) {
            free(gen->quads);
        }
        if (gen->asmLines != NULL) {
            free(gen->asmLines);
        }
        free(gen);
    }
}

/* 辅助函数：添加汇编代码行 */
static void addAsmLine(CodeGenerator *gen, const char *line) {
    if (gen == NULL || line == NULL) return;
    
    if (gen->asmLines == NULL) {
        gen->asmCapacity = 1000;
        gen->asmLines = (AsmLine *)malloc(gen->asmCapacity * sizeof(AsmLine));
        gen->asmCount = 0;
    }
    
    if (gen->asmCount >= gen->asmCapacity) {
        gen->asmCapacity *= 2;
        gen->asmLines = (AsmLine *)realloc(gen->asmLines, gen->asmCapacity * sizeof(AsmLine));
    }
    
    strncpy(gen->asmLines[gen->asmCount].code, line, 255);
    gen->asmLines[gen->asmCount].code[255] = '\0';
    gen->asmCount++;
}

/* 生成汇编代码 */
void generateAssemblyCode(CodeGenerator *gen, SymbolTable *table) {
    if (gen == NULL) return;
    
    char line[256];
    
    /* 汇编头部 */
    addAsmLine(gen, ";;; =======================================");
    addAsmLine(gen, ";;; Generated Assembly Code");
    addAsmLine(gen, ";;; =======================================");
    addAsmLine(gen, "");
    addAsmLine(gen, ".386");
    addAsmLine(gen, ".model flat, stdcall");
    addAsmLine(gen, "");
    
    /* 数据段 */
    addAsmLine(gen, ".data");
    
    /* 添加变量声明 */
    if (table != NULL) {
        for (int i = 0; i < table->count; i++) {
            if (table->symbols[i].kind == SYM_VARIABLE) {
                sprintf(line, "    %s dd 0", table->symbols[i].name);
                addAsmLine(gen, line);
            }
        }
    }
    addAsmLine(gen, "");
    
    /* 代码段 */
    addAsmLine(gen, ".code");
    addAsmLine(gen, "");
    
    /* 处理每个四元式 */
    for (int i = 0; i < gen->count; i++) {
        Quadruple *q = &gen->quads[i];
        
        if (strcmp(q->op, "DECL") == 0) {
            /* 变量声明 - 汇编中已在数据段处理 */
            sprintf(line, "    ;;; DECL %s", q->arg1);
            addAsmLine(gen, line);
        }
        else if (strcmp(q->op, "=") == 0) {
            /* 赋值操作 */
            if (atoi(q->arg1) > -100 && atoi(q->arg1) < 100) {
                /* 常数 */
                sprintf(line, "    mov eax, %s", q->arg1);
            } else {
                /* 变量 */
                sprintf(line, "    mov eax, [%s]", q->arg1);
            }
            addAsmLine(gen, line);
            sprintf(line, "    mov [%s], eax", q->result);
            addAsmLine(gen, line);
        }
        else if (strcmp(q->op, "+") == 0) {
            /* 加法 */
            sprintf(line, "    mov eax, [%s]", q->arg1);
            addAsmLine(gen, line);
            sprintf(line, "    mov ebx, [%s]", q->arg2);
            addAsmLine(gen, line);
            addAsmLine(gen, "    add eax, ebx");
            sprintf(line, "    mov [%s], eax", q->result);
            addAsmLine(gen, line);
        }
        else if (strcmp(q->op, "-") == 0) {
            /* 减法 */
            sprintf(line, "    mov eax, [%s]", q->arg1);
            addAsmLine(gen, line);
            sprintf(line, "    mov ebx, [%s]", q->arg2);
            addAsmLine(gen, line);
            addAsmLine(gen, "    sub eax, ebx");
            sprintf(line, "    mov [%s], eax", q->result);
            addAsmLine(gen, line);
        }
        else if (strcmp(q->op, "*") == 0) {
            /* 乘法 */
            sprintf(line, "    mov eax, [%s]", q->arg1);
            addAsmLine(gen, line);
            sprintf(line, "    mov ebx, [%s]", q->arg2);
            addAsmLine(gen, line);
            addAsmLine(gen, "    imul eax, ebx");
            sprintf(line, "    mov [%s], eax", q->result);
            addAsmLine(gen, line);
        }
        else if (strcmp(q->op, "/") == 0) {
            /* 除法 */
            sprintf(line, "    mov eax, [%s]", q->arg1);
            addAsmLine(gen, line);
            sprintf(line, "    mov ebx, [%s]", q->arg2);
            addAsmLine(gen, line);
            addAsmLine(gen, "    cdq");
            addAsmLine(gen, "    idiv ebx");
            sprintf(line, "    mov [%s], eax", q->result);
            addAsmLine(gen, line);
        }
        else if (strcmp(q->op, "WRITE") == 0) {
            /* 输出语句 */
            sprintf(line, "    ;;; WRITE [%s]", q->arg1);
            addAsmLine(gen, line);
            sprintf(line, "    mov eax, [%s]", q->arg1);
            addAsmLine(gen, line);
            addAsmLine(gen, "    ;;; 需要调用输出函数（如 printf）");
        }
        else if (strcmp(q->op, "READ") == 0) {
            /* 输入语句 */
            sprintf(line, "    ;;; READ [%s]", q->arg1);
            addAsmLine(gen, line);
            addAsmLine(gen, "    ;;; 需要调用输入函数（如 scanf）");
        }
        else if (strcmp(q->op, "PARAM") == 0) {
            /* 函数参数传递 */
            if (strlen(q->arg1) > 0) {
                sprintf(line, "    mov eax, [%s]", q->arg1);
                addAsmLine(gen, line);
            } else {
                sprintf(line, "    mov eax, %s", q->arg1);
                addAsmLine(gen, line);
            }
            addAsmLine(gen, "    push eax");
            sprintf(line, "    ;;; Push parameter: %s", q->arg1);
            addAsmLine(gen, line);
        }
        else if (strncmp(q->op, "CALL", 4) == 0) {
            /* 函数调用 */
            char funcName[128];
            sscanf(q->op, "CALL %s", funcName);
            sprintf(line, "    call %s", funcName);
            addAsmLine(gen, line);
            sprintf(line, "    mov [%s], eax", q->result);
            addAsmLine(gen, line);
        }
        else {
            /* 其他操作符 */
            sprintf(line, "    ;;; %s %s, %s -> %s", q->op, q->arg1, q->arg2, q->result);
            addAsmLine(gen, line);
        }
    }
    
    /* 程序结束 */
    addAsmLine(gen, "");
    addAsmLine(gen, "    ;;; 程序结束");
    addAsmLine(gen, "    mov eax, 0");
    addAsmLine(gen, "    ret");
    addAsmLine(gen, "");
    addAsmLine(gen, "end");
}

/* 打印汇编代码到控制台 */
void printAssemblyCode(CodeGenerator *gen) {
    if (gen == NULL) return;
    
    printf("\n========== Assembly Code ==========\n");
    for (int i = 0; i < gen->asmCount; i++) {
        printf("%s\n", gen->asmLines[i].code);
    }
    printf("===================================\n\n");
}

/* 打印汇编代码到文件 */
void printAssemblyCodeToFile(CodeGenerator *gen, FILE *file) {
    if (gen == NULL || file == NULL) return;
    
    fprintf(file, "\n========== Assembly Code ==========\n");
    for (int i = 0; i < gen->asmCount; i++) {
        fprintf(file, "%s\n", gen->asmLines[i].code);
    }
    fprintf(file, "===================================\n\n");
}
