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
        free(gen);
    }
}
