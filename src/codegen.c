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
                char tempVar_buf[32];
                char *tempVar = getTempVar(gen);
                strcpy(tempVar_buf, tempVar);
                generateIntermediateCodeExpr(ast->children[1], gen, table, tempVar_buf);
                genCode(gen, "=", tempVar_buf, "", result);
            }
            break;
            
        case NODE_WRITE:
            if (ast->childrenCount > 0) {
                char tempVar_buf[32];
                char *tempVar = getTempVar(gen);
                strcpy(tempVar_buf, tempVar);
                generateIntermediateCodeExpr(ast->children[0], gen, table, tempVar_buf);
                genCode(gen, "WRITE", tempVar_buf, "", "");
            }
            break;
            
        case NODE_READ:
            if (ast->childrenCount > 0) {
                genCode(gen, "READ", ast->children[0]->name, "", "");
            }
            break;
            
        case NODE_FUNC_DEF: {
            /* 函数定义：funcName, [paramList], stmtList */
            if (ast->childrenCount >= 2) {
                ASTNode *funcName = ast->children[0];
                ASTNode *paramList = NULL;
                ASTNode *stmtList = NULL;
                
                /* 判断是否有参数列表 */
                if (ast->childrenCount == 3) {
                    /* 有参数列表: children[0]=funcName, children[1]=paramList, children[2]=stmtList */
                    paramList = ast->children[1];
                    stmtList = ast->children[2];
                } else {
                    /* 无参数列表: children[0]=funcName, children[1]=stmtList */
                    stmtList = ast->children[1];
                }
                
                if (funcName->type == NODE_ID) {
                    /* 生成函数标签 */
                    char labelBuf[256];
                    sprintf(labelBuf, "LABEL %s", funcName->name);
                    genCode(gen, labelBuf, "", "", "");
                    
                    /* 生成参数声明 */
                    if (paramList != NULL && paramList->type == NODE_PARAM_LIST) {
                        for (int i = 0; i < paramList->childrenCount; i++) {
                            ASTNode *param = paramList->children[i];
                            if (param->type == NODE_DECL && param->childrenCount > 0) {
                                ASTNode *paramId = param->children[0];
                                if (paramId->type == NODE_ID) {
                                    genCode(gen, "DECL", paramId->name, "", "");
                                }
                            }
                        }
                    }
                    
                    /* 生成函数体代码 */
                    if (stmtList != NULL) {
                        generateIntermediateCode(stmtList, gen, table);
                    }
                    
                    /* 生成函数返回指令 */
                    genCode(gen, "RET", "", "", "");
                }
            }
            break;
        }
            
        case NODE_RETURN_STMT: {
            /* 返回语句 */
            if (ast->childrenCount > 0) {
                char tempVar_buf[32];
                char *tempVar = getTempVar(gen);
                strcpy(tempVar_buf, tempVar);
                generateIntermediateCodeExpr(ast->children[0], gen, table, tempVar_buf);
                genCode(gen, "RET", tempVar_buf, "", "");
            } else {
                genCode(gen, "RET", "", "", "");
            }
            break;
        }
            
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
                char temp1_buf[32], temp2_buf[32];
                temp1 = getTempVar(gen);
                strcpy(temp1_buf, temp1);
                temp2 = getTempVar(gen);
                strcpy(temp2_buf, temp2);
                generateIntermediateCodeExpr(ast->children[0], gen, table, temp1_buf);
                generateIntermediateCodeExpr(ast->children[1], gen, table, temp2_buf);
                genCode(gen, getOpString(ast->op), temp1_buf, temp2_buf, result);
            }
            break;
        }
            
        case NODE_UNOP:
            if (ast->childrenCount > 0) {
                char temp1_buf[32];
                temp1 = getTempVar(gen);
                strcpy(temp1_buf, temp1);
                generateIntermediateCodeExpr(ast->children[0], gen, table, temp1_buf);
                genCode(gen, getOpString(ast->op), temp1_buf, "", result);
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
                            char argTemp_buf[32];
                            char *argTemp = getTempVar(gen);
                            strcpy(argTemp_buf, argTemp);
                            generateIntermediateCodeExpr(argList->children[i], gen, table, argTemp_buf);
                            /* PARAM 操作符，参数在 arg1 */
                            genCode(gen, "PARAM", argTemp_buf, "", "");
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

/* ==================== 代码优化函数 ==================== */

/* 检查字符串是否为常数 */
int isConstant(const char *str) {
    if (str == NULL || strlen(str) == 0) return 0;
    char *endptr;
    strtol(str, &endptr, 10);
    return *endptr == '\0';
}

/* 计算常数表达式的值 */
int evaluateConstant(const char *op, int val1, int val2) {
    if (strcmp(op, "+") == 0) return val1 + val2;
    if (strcmp(op, "-") == 0) return val1 - val2;
    if (strcmp(op, "*") == 0) return val1 * val2;
    if (strcmp(op, "/") == 0 && val2 != 0) return val1 / val2;
    return 0; // 无效操作或除零
}

/* 检查四元式是否为死代码 */
int isDeadCode(Quadruple *q, CodeGenerator *gen, int index) {
    if (q == NULL || gen == NULL) return 0;

    /* 如果是赋值给临时变量，检查后续是否使用 */
    if (strcmp(q->op, "=") == 0 && strncmp(q->result, "t", 1) == 0) {
        for (int i = index + 1; i < gen->count; i++) {
            Quadruple *next = &gen->quads[i];
            /* 检查是否在后续四元式中使用 */
            if (strcmp(next->arg1, q->result) == 0 ||
                strcmp(next->arg2, q->result) == 0 ||
                strcmp(next->result, q->result) == 0) {
                return 0; /* 被使用，不是死代码 */
            }
        }
        return 1; /* 未被使用，是死代码 */
    }
    return 0;
}

/* 移除指定索引的四元式 */
void removeQuadruple(CodeGenerator *gen, int index) {
    if (gen == NULL || index < 0 || index >= gen->count) return;

    /* 移动后续四元式向前 */
    for (int i = index; i < gen->count - 1; i++) {
        gen->quads[i] = gen->quads[i + 1];
    }
    gen->count--;
}

/* 主要优化函数 */
void optimizeIntermediateCode(CodeGenerator *gen) {
    if (gen == NULL || gen->count == 0) return;

    int optimized = 1; /* 标记是否进行了优化 */
    while (optimized) {
        optimized = 0;

        /* Pass 1: 常数折叠 */
        for (int i = 0; i < gen->count; i++) {
            Quadruple *q = &gen->quads[i];

            /* 检查二元运算的常数折叠 */
            if ((strcmp(q->op, "+") == 0 || strcmp(q->op, "-") == 0 ||
                 strcmp(q->op, "*") == 0 || strcmp(q->op, "/") == 0) &&
                isConstant(q->arg1) && isConstant(q->arg2)) {

                int val1 = atoi(q->arg1);
                int val2 = atoi(q->arg2);
                int result = evaluateConstant(q->op, val1, val2);

                /* 替换为常数赋值 */
                sprintf(q->op, "=");
                sprintf(q->arg1, "%d", result);
                sprintf(q->arg2, "");
                optimized = 1;
            }

            /* 检查代数简化 */
            if (strcmp(q->op, "+") == 0 && strcmp(q->arg2, "0") == 0) {
                /* x + 0 = x */
                sprintf(q->op, "=");
                sprintf(q->arg2, "");
                optimized = 1;
            }
            else if (strcmp(q->op, "-") == 0 && strcmp(q->arg2, "0") == 0) {
                /* x - 0 = x */
                sprintf(q->op, "=");
                sprintf(q->arg2, "");
                optimized = 1;
            }
            else if (strcmp(q->op, "*") == 0 && strcmp(q->arg2, "1") == 0) {
                /* x * 1 = x */
                sprintf(q->op, "=");
                sprintf(q->arg2, "");
                optimized = 1;
            }
            else if (strcmp(q->op, "*") == 0 && strcmp(q->arg1, "1") == 0) {
                /* 1 * x = x */
                sprintf(q->op, "=");
                sprintf(q->arg1, "%s", q->arg2);
                sprintf(q->arg2, "");
                optimized = 1;
            }
        }

        /* Pass 2: 死代码消除 */
        for (int i = 0; i < gen->count; i++) {
            if (isDeadCode(&gen->quads[i], gen, i)) {
                removeQuadruple(gen, i);
                optimized = 1;
                i--; /* 重新检查当前位置 */
            }
        }

        /* Pass 3: 复制传播 */
        for (int i = 0; i < gen->count; i++) {
            Quadruple *q = &gen->quads[i];

            /* 如果是简单的赋值 t = x */
            if (strcmp(q->op, "=") == 0 && strlen(q->arg2) == 0 &&
                strncmp(q->result, "t", 1) == 0) {

                char *tempVar = q->result;
                char *source = q->arg1;

                /* 在后续四元式中传播 */
                for (int j = i + 1; j < gen->count; j++) {
                    Quadruple *next = &gen->quads[j];

                    /* 替换使用这个临时变量的地方 */
                    if (strcmp(next->arg1, tempVar) == 0) {
                        strcpy(next->arg1, source);
                        optimized = 1;
                    }
                    if (strcmp(next->arg2, tempVar) == 0) {
                        strcpy(next->arg2, source);
                        optimized = 1;
                    }

                    /* 如果遇到对这个临时变量的重新赋值，停止传播 */
                    if (strcmp(next->result, tempVar) == 0) {
                        break;
                    }
                }
            }
        }
    }
}

/* ==================== 汇编代码优化函数 ==================== */

/* 提取汇编指令中的操作数 */
int extractAsmOperands(const char *code, char *op, char *reg1, char *reg2) {
    if (code == NULL || strlen(code) < 4) return 0;
    
    /* 跳过前导空格和注释 */
    while (*code && (*code == ' ' || *code == '\t' || *code == ';')) code++;
    
    /* 提取指令 */
    int i = 0;
    while (i < 31 && code[i] && code[i] != ' ' && code[i] != '\t') {
        op[i] = code[i];
        i++;
    }
    op[i] = '\0';
    
    if (strlen(op) == 0) return 0;
    
    return 1;
}

/* 检查两条指令是否为冗余mov */
int isRedundantMov(const char *prev_code, const char *curr_code) {
    if (prev_code == NULL || curr_code == NULL) return 0;
    
    /* mov [a], eax; mov eax, [a] -> 冗余 */
    /* mov eax, [a]; mov eax, [a] -> 冗余 */
    
    /* 简化：检查是否完全相同 */
    const char *p = prev_code;
    const char *c = curr_code;
    
    /* 跳过前导空格 */
    while (*p && (*p == ' ' || *p == '\t')) p++;
    while (*c && (*c == ' ' || *c == '\t')) c++;
    
    /* 检查是否是形如 "mov eax, [x]; mov eax, [x]" 的重复 */
    if (strncmp(p, "mov", 3) == 0 && strncmp(c, "mov", 3) == 0) {
        const char *rest_p = p + 3;
        const char *rest_c = c + 3;
        
        while (*rest_p && (*rest_p == ' ' || *rest_p == '\t')) rest_p++;
        while (*rest_c && (*rest_c == ' ' || *rest_c == '\t')) rest_c++;
        
        /* 检查操作数是否相同 */
        return strcmp(rest_p, rest_c) == 0;
    }
    
    return 0;
}

/* 检查是否为无效指令 */
int isInvalidInstruction(const char *code) {
    if (code == NULL || strlen(code) == 0) return 0;
    
    const char *p = code;
    while (*p && (*p == ' ' || *p == '\t')) p++;
    
    /* 空行 */
    if (*p == '\0') return 1;
    
    /* 仅注释 */
    if (*p == ';') return 1;
    
    /* mov eax, eax 冗余 */
    if (strncmp(p, "mov eax, eax", 12) == 0) return 1;
    if (strncmp(p, "mov ebx, ebx", 12) == 0) return 1;
    if (strncmp(p, "mov ecx, ecx", 12) == 0) return 1;
    if (strncmp(p, "mov edx, edx", 12) == 0) return 1;
    
    return 0;
}

/* 汇编代码优化 */
void optimizeAssemblyCode(CodeGenerator *gen) {
    if (gen == NULL || gen->asmCount == 0) return;
    
    int optimized = 1;
    while (optimized) {
        optimized = 0;
        
        /* Pass 1: 移除无效指令 */
        for (int i = 0; i < gen->asmCount; i++) {
            if (isInvalidInstruction(gen->asmLines[i].code)) {
                /* 移动后续指令向前 */
                for (int j = i; j < gen->asmCount - 1; j++) {
                    strcpy(gen->asmLines[j].code, gen->asmLines[j + 1].code);
                }
                gen->asmCount--;
                optimized = 1;
                i--;
            }
        }
        
        /* Pass 2: 移除冗余mov指令 */
        for (int i = 0; i < gen->asmCount - 1; i++) {
            /* 检查当前指令是否是mov */
            if (strncmp(gen->asmLines[i].code, "    mov", 7) == 0) {
                /* 检查下一条指令是否也是mov且为冗余 */
                if (i + 1 < gen->asmCount && 
                    isRedundantMov(gen->asmLines[i].code, gen->asmLines[i + 1].code)) {
                    /* 移除冗余指令 */
                    for (int j = i + 1; j < gen->asmCount - 1; j++) {
                        strcpy(gen->asmLines[j].code, gen->asmLines[j + 1].code);
                    }
                    gen->asmCount--;
                    optimized = 1;
                }
            }
        }
        
        /* Pass 3: 移除push-pop对 (当操作相同的寄存器时) */
        for (int i = 0; i < gen->asmCount - 1; i++) {
            const char *curr = gen->asmLines[i].code;
            const char *next = gen->asmLines[i + 1].code;
            
            /* push eax; ...; pop eax (中间没有改变eax) */
            if (strncmp(curr, "    push eax", 12) == 0 &&
                strncmp(next, "    pop eax", 11) == 0) {
                
                /* 检查中间是否有改变eax的指令 */
                int can_remove = 1;
                for (int j = i + 1; j <= i + 1; j++) {
                    const char *code = gen->asmLines[j].code;
                    if (strstr(code, "eax") && 
                        (strstr(code, "mov eax") || strstr(code, "add eax") || 
                         strstr(code, "sub eax") || strstr(code, "imul eax"))) {
                        can_remove = 0;
                        break;
                    }
                }
                
                if (can_remove && i + 1 < gen->asmCount) {
                    /* 移除push和pop */
                    for (int j = i; j < gen->asmCount - 2; j++) {
                        strcpy(gen->asmLines[j].code, gen->asmLines[j + 2].code);
                    }
                    gen->asmCount -= 2;
                    optimized = 1;
                }
            }
        }
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
            if (strlen(q->result) > 0) {
                sprintf(line, "    mov [%s], eax", q->result);
                addAsmLine(gen, line);
            }
        }
        else if (strncmp(q->op, "LABEL", 5) == 0) {
            /* 函数标签 */
            char funcName[128];
            sscanf(q->op, "LABEL %s", funcName);
            sprintf(line, "%s:", funcName);
            addAsmLine(gen, line);
        }
        else if (strcmp(q->op, "RET") == 0) {
            /* 函数返回 */
            if (strlen(q->arg1) > 0) {
                /* 有返回值 */
                sprintf(line, "    mov eax, [%s]", q->arg1);
                addAsmLine(gen, line);
            }
            addAsmLine(gen, "    ret");
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
