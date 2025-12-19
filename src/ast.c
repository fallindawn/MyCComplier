#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode* createNode(NodeType type) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = type;
    node->dataType = TYPE_UNKNOWN;
    node->intValue = 0;
    node->stringValue = NULL;
    node->name = NULL;
    node->op = 0;
    node->children = NULL;
    node->childrenCount = 0;
    node->childrenCapacity = 0;
    return node;
}

ASTNode* createNumNode(int value) {
    ASTNode *node = createNode(NODE_NUM);
    node->intValue = value;
    node->dataType = TYPE_INT;
    return node;
}

ASTNode* createIdNode(char *name) {
    ASTNode *node = createNode(NODE_ID);
    node->name = (char *)malloc(strlen(name) + 1);
    strcpy(node->name, name);
    return node;
}

ASTNode* createBinOpNode(OpType op, ASTNode *left, ASTNode *right) {
    ASTNode *node = createNode(NODE_BINOP);
    node->op = op;
    addChild(node, left);
    addChild(node, right);
    return node;
}

ASTNode* createUnOpNode(OpType op, ASTNode *operand) {
    ASTNode *node = createNode(NODE_UNOP);
    node->op = op;
    addChild(node, operand);
    return node;
}

void addChild(ASTNode *parent, ASTNode *child) {
    if (parent == NULL || child == NULL) return;
    
    if (parent->childrenCount >= parent->childrenCapacity) {
        parent->childrenCapacity = (parent->childrenCapacity == 0) ? 4 : parent->childrenCapacity * 2;
        parent->children = (ASTNode **)realloc(parent->children, 
                          parent->childrenCapacity * sizeof(ASTNode *));
    }
    
    parent->children[parent->childrenCount++] = child;
}

void freeAST(ASTNode *node) {
    if (node == NULL) return;
    
    for (int i = 0; i < node->childrenCount; i++) {
        freeAST(node->children[i]);
    }
    
    if (node->children != NULL) {
        free(node->children);
    }
    
    if (node->name != NULL) {
        free(node->name);
    }
    
    if (node->stringValue != NULL) {
        free(node->stringValue);
    }
    
    free(node);
}

const char* getNodeTypeName(NodeType type) {
    switch (type) {
        case NODE_PROGRAM: return "PROGRAM";
        case NODE_DECL_LIST: return "DECL_LIST";
        case NODE_DECL: return "DECL";
        case NODE_STMT_LIST: return "STMT_LIST";
        case NODE_STMT: return "STMT";
        case NODE_ASSIGN: return "ASSIGN";
        case NODE_IF: return "IF";
        case NODE_WHILE: return "WHILE";
        case NODE_FOR: return "FOR";
        case NODE_READ: return "READ";
        case NODE_WRITE: return "WRITE";
        case NODE_EXPR: return "EXPR";
        case NODE_BINOP: return "BINOP";
        case NODE_UNOP: return "UNOP";
        case NODE_NUM: return "NUM";
        case NODE_ID: return "ID";
        case NODE_CALL: return "CALL";
        case NODE_ARG_LIST: return "ARG_LIST";
        default: return "UNKNOWN";
    }
}

const char* getOpName(OpType op) {
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

void printAST(ASTNode *node, int indent) {
    if (node == NULL) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    printf("%s", getNodeTypeName(node->type));
    
    if (node->type == NODE_NUM) {
        printf("(%d)", node->intValue);
    } else if (node->type == NODE_ID) {
        printf("(%s)", node->name);
    } else if (node->type == NODE_BINOP || node->type == NODE_UNOP) {
        printf("(%s)", getOpName(node->op));
    }
    
    printf("\n");
    
    for (int i = 0; i < node->childrenCount; i++) {
        printAST(node->children[i], indent + 1);
    }
}

void printASTToFile(ASTNode *node, int indent, FILE *file) {
    if (node == NULL || file == NULL) return;
    
    for (int i = 0; i < indent; i++) fprintf(file, "  ");
    fprintf(file, "%s", getNodeTypeName(node->type));
    
    if (node->type == NODE_NUM) {
        fprintf(file, "(%d)", node->intValue);
    } else if (node->type == NODE_ID) {
        fprintf(file, "(%s)", node->name);
    } else if (node->type == NODE_BINOP || node->type == NODE_UNOP) {
        fprintf(file, "(%s)", getOpName(node->op));
    }
    
    fprintf(file, "\n");
    
    for (int i = 0; i < node->childrenCount; i++) {
        printASTToFile(node->children[i], indent + 1, file);
    }
}
