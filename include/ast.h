#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <string.h>

/* 节点类型枚举 */
typedef enum {
    NODE_PROGRAM,
    NODE_DECL_LIST,
    NODE_DECL,
    NODE_STMT_LIST,
    NODE_STMT,
    NODE_ASSIGN,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_READ,
    NODE_WRITE,
    NODE_EXPR,
    NODE_BINOP,
    NODE_UNOP,
    NODE_NUM,
    NODE_ID,
    NODE_CALL,
    NODE_ARG_LIST
} NodeType;

/* 运算符类型 */
typedef enum {
    OP_PLUS,
    OP_MINUS,
    OP_MULT,
    OP_DIV,
    OP_MOD,
    OP_POW,
    OP_EQ,
    OP_NE,
    OP_LT,
    OP_GT,
    OP_LE,
    OP_GE,
    OP_AND,
    OP_OR,
    OP_NOT
} OpType;

/* 数据类型 */
typedef enum {
    TYPE_INT,
    TYPE_UNKNOWN
} DataType;

/* AST 节点结构 */
typedef struct ASTNode {
    NodeType type;
    DataType dataType;
    
    /* 数据字段 */
    int intValue;
    char *stringValue;
    char *name;
    OpType op;
    
    /* 子节点 */
    struct ASTNode **children;
    int childrenCount;
    int childrenCapacity;
} ASTNode;

/* 函数声明 */
ASTNode* createNode(NodeType type);
ASTNode* createNumNode(int value);
ASTNode* createIdNode(char *name);
ASTNode* createBinOpNode(OpType op, ASTNode *left, ASTNode *right);
ASTNode* createUnOpNode(OpType op, ASTNode *operand);
void addChild(ASTNode *parent, ASTNode *child);
void freeAST(ASTNode *node);
void printAST(ASTNode *node, int indent);

#endif
