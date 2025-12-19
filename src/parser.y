%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

/* Type definitions for Bison union - must be included before y.tab.h */
/* These will be generated, so we include the necessary definitions */
#include "ast.h"

extern int lineNum;
extern int yylex();
extern int yyerror(const char *msg);

/* Function declarations */
ASTNode* createNode(NodeType type);
ASTNode* createNumNode(int value);
ASTNode* createIdNode(char *name);
ASTNode* createBinOpNode(OpType op, ASTNode *left, ASTNode *right);
ASTNode* createUnOpNode(OpType op, ASTNode *operand);
void addChild(ASTNode *parent, ASTNode *child);

/* Include other headers after types are defined */
#include "symbol_table.h"
#include "codegen.h"

ASTNode *root = NULL;
SymbolTable *symTable = NULL;
CodeGenerator *codeGen = NULL;

%}

%union {
    int num;
    char *id;
    char *str;
    ASTNode *node;
}

%token INT FLOAT DOUBLE CHAR SHORT LONG UNSIGNED BOOL VOID IF ELSE WHILE FOR SCANF PRINTF RETURN
%token PLUS MINUS MULT DIV MOD POW
%token EQ NE LT GT LE GE
%token AND OR NOT
%token ASSIGN
%token LPAREN RPAREN LBRACE RBRACE SEMI COMMA
%token <num> NUM
%token <id> ID
%token <str> STRING

%type <node> program decl_list decl stmt_list stmt func_decl_list func_decl param_list param
%type <node> assign_stmt if_stmt while_stmt for_stmt
%type <node> read_stmt write_stmt return_stmt func_call_stmt arg_list expr
%type <node> additive_expr multiplicative_expr
%type <node> unary_expr primary_expr comparison_expr
%type <node> logical_and_expr logical_or_expr

%left OR
%left AND
%left EQ NE LT GT LE GE
%left PLUS MINUS
%left MULT DIV MOD
%right POW
%right NOT
%nonassoc UMINUS

%%

program
    : func_decl_list decl_list stmt_list
        {
            $$ = createNode(NODE_PROGRAM);
            addChild($$, $1);
            addChild($$, $2);
            addChild($$, $3);
            root = $$;
        }
    | func_decl_list stmt_list
        {
            $$ = createNode(NODE_PROGRAM);
            addChild($$, $1);
            addChild($$, $2);
            root = $$;
        }
    | decl_list stmt_list
        {
            $$ = createNode(NODE_PROGRAM);
            addChild($$, $1);
            addChild($$, $2);
            root = $$;
        }
    | stmt_list
        {
            $$ = createNode(NODE_PROGRAM);
            addChild($$, $1);
            root = $$;
        }
    | func_decl_list
        {
            $$ = createNode(NODE_PROGRAM);
            addChild($$, $1);
            root = $$;
        }
    ;

func_decl_list
    : func_decl
        {
            $$ = createNode(NODE_DECL_LIST);
            addChild($$, $1);
        }
    | func_decl_list func_decl
        {
            addChild($1, $2);
            $$ = $1;
        }
    ;

func_decl
    : INT ID LPAREN param_list RPAREN LBRACE stmt_list RBRACE
        {
            insertFunction(symTable, $2, TYPE_INT, lineNum);
            $$ = createNode(NODE_FUNC_DEF);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            if ($4 != NULL) addChild($$, $4);
            addChild($$, $7);
        }
    | INT ID LPAREN RPAREN LBRACE stmt_list RBRACE
        {
            insertFunction(symTable, $2, TYPE_INT, lineNum);
            $$ = createNode(NODE_FUNC_DEF);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            addChild($$, $6);
        }
    | FLOAT ID LPAREN param_list RPAREN LBRACE stmt_list RBRACE
        {
            insertFunction(symTable, $2, TYPE_FLOAT, lineNum);
            $$ = createNode(NODE_FUNC_DEF);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            if ($4 != NULL) addChild($$, $4);
            addChild($$, $7);
        }
    | FLOAT ID LPAREN RPAREN LBRACE stmt_list RBRACE
        {
            insertFunction(symTable, $2, TYPE_FLOAT, lineNum);
            $$ = createNode(NODE_FUNC_DEF);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            addChild($$, $6);
        }
    | DOUBLE ID LPAREN param_list RPAREN LBRACE stmt_list RBRACE
        {
            insertFunction(symTable, $2, TYPE_DOUBLE, lineNum);
            $$ = createNode(NODE_FUNC_DEF);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            if ($4 != NULL) addChild($$, $4);
            addChild($$, $7);
        }
    | DOUBLE ID LPAREN RPAREN LBRACE stmt_list RBRACE
        {
            insertFunction(symTable, $2, TYPE_DOUBLE, lineNum);
            $$ = createNode(NODE_FUNC_DEF);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            addChild($$, $6);
        }
    | VOID ID LPAREN param_list RPAREN LBRACE stmt_list RBRACE
        {
            insertFunction(symTable, $2, TYPE_VOID, lineNum);
            $$ = createNode(NODE_FUNC_DEF);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            if ($4 != NULL) addChild($$, $4);
            addChild($$, $7);
        }
    | VOID ID LPAREN RPAREN LBRACE stmt_list RBRACE
        {
            insertFunction(symTable, $2, TYPE_VOID, lineNum);
            $$ = createNode(NODE_FUNC_DEF);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            addChild($$, $6);
        }
    ;

param_list
    : param
        {
            $$ = createNode(NODE_PARAM_LIST);
            addChild($$, $1);
        }
    | param_list COMMA param
        {
            addChild($1, $3);
            $$ = $1;
        }
    ;

param
    : INT ID
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            insertSymbol(symTable, $2, TYPE_INT, lineNum);
        }
    | FLOAT ID
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            insertSymbol(symTable, $2, TYPE_FLOAT, lineNum);
        }
    | DOUBLE ID
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            insertSymbol(symTable, $2, TYPE_DOUBLE, lineNum);
        }
    | CHAR ID
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            insertSymbol(symTable, $2, TYPE_CHAR, lineNum);
        }
    ;

decl_list
    : decl
        {
            $$ = createNode(NODE_DECL_LIST);
            addChild($$, $1);
        }
    | decl_list decl
        {
            addChild($1, $2);
            $$ = $1;
        }
    ;

decl
    : INT ID SEMI
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            insertSymbol(symTable, $2, TYPE_INT, lineNum);
        }
    | FLOAT ID SEMI
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            insertSymbol(symTable, $2, TYPE_FLOAT, lineNum);
        }
    | DOUBLE ID SEMI
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            insertSymbol(symTable, $2, TYPE_DOUBLE, lineNum);
        }
    | CHAR ID SEMI
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            insertSymbol(symTable, $2, TYPE_CHAR, lineNum);
        }
    | SHORT ID SEMI
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            insertSymbol(symTable, $2, TYPE_SHORT, lineNum);
        }
    | LONG ID SEMI
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            insertSymbol(symTable, $2, TYPE_LONG, lineNum);
        }
    | LONG LONG ID SEMI
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($3);
            addChild($$, idNode);
            insertSymbol(symTable, $3, TYPE_LONG_LONG, lineNum);
        }
    | BOOL ID SEMI
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            insertSymbol(symTable, $2, TYPE_BOOL, lineNum);
        }
    | VOID ID SEMI
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($2);
            addChild($$, idNode);
            insertSymbol(symTable, $2, TYPE_VOID, lineNum);
        }
    | UNSIGNED INT ID SEMI
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($3);
            addChild($$, idNode);
            insertSymbol(symTable, $3, TYPE_UNSIGNED_INT, lineNum);
        }
    | UNSIGNED CHAR ID SEMI
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($3);
            addChild($$, idNode);
            insertSymbol(symTable, $3, TYPE_UNSIGNED_CHAR, lineNum);
        }
    | UNSIGNED SHORT ID SEMI
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($3);
            addChild($$, idNode);
            insertSymbol(symTable, $3, TYPE_UNSIGNED_SHORT, lineNum);
        }
    | UNSIGNED LONG ID SEMI
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($3);
            addChild($$, idNode);
            insertSymbol(symTable, $3, TYPE_UNSIGNED_LONG, lineNum);
        }
    | UNSIGNED LONG LONG ID SEMI
        {
            $$ = createNode(NODE_DECL);
            ASTNode *idNode = createIdNode($4);
            addChild($$, idNode);
            insertSymbol(symTable, $4, TYPE_UNSIGNED_LONG_LONG, lineNum);
        }
    ;

stmt_list
    : stmt
        {
            $$ = createNode(NODE_STMT_LIST);
            addChild($$, $1);
        }
    | decl
        {
            $$ = createNode(NODE_STMT_LIST);
            addChild($$, $1);
        }
    | stmt_list stmt
        {
            addChild($1, $2);
            $$ = $1;
        }
    | stmt_list decl
        {
            addChild($1, $2);
            $$ = $1;
        }
    ;

stmt
    : LBRACE stmt_list RBRACE
        {
            $$ = $2;
        }
    | assign_stmt { $$ = $1; }
    | if_stmt { $$ = $1; }
    | while_stmt { $$ = $1; }
    | for_stmt { $$ = $1; }
    | read_stmt { $$ = $1; }
    | write_stmt { $$ = $1; }
    | return_stmt { $$ = $1; }
    | func_call_stmt { $$ = $1; }
    ;

assign_stmt
    : ID ASSIGN expr SEMI
        {
            if (!isSymbolDefined(symTable, $1)) {
                fprintf(stderr, "Error at line %d: Variable '%s' not declared\n", lineNum, $1);
            }
            $$ = createNode(NODE_ASSIGN);
            ASTNode *idNode = createIdNode($1);
            addChild($$, idNode);
            addChild($$, $3);
        }
    ;

if_stmt
    : IF LPAREN expr RPAREN stmt %prec IF
        {
            $$ = createNode(NODE_IF);
            addChild($$, $3);
            addChild($$, $5);
        }
    | IF LPAREN expr RPAREN stmt ELSE stmt
        {
            $$ = createNode(NODE_IF);
            addChild($$, $3);
            addChild($$, $5);
            addChild($$, $7);
        }
    ;

while_stmt
    : WHILE LPAREN expr RPAREN stmt
        {
            $$ = createNode(NODE_WHILE);
            addChild($$, $3);
            addChild($$, $5);
        }
    ;

for_stmt
    : FOR LPAREN ID ASSIGN expr SEMI expr SEMI ID ASSIGN expr RPAREN stmt
        {
            $$ = createNode(NODE_FOR);
            ASTNode *init = createNode(NODE_ASSIGN);
            addChild(init, createIdNode($3));
            addChild(init, $5);
            addChild($$, init);
            addChild($$, $7);
            ASTNode *update = createNode(NODE_ASSIGN);
            addChild(update, createIdNode($9));
            addChild(update, $11);
            addChild($$, update);
            addChild($$, $13);
        }
    ;

read_stmt
    : SCANF LPAREN STRING COMMA ID RPAREN SEMI
        {
            if (!isSymbolDefined(symTable, $5)) {
                fprintf(stderr, "Error at line %d: Variable '%s' not declared\n", lineNum, $5);
            }
            $$ = createNode(NODE_READ);
            ASTNode *idNode = createIdNode($5);
            addChild($$, idNode);
        }
    ;

write_stmt
    : PRINTF LPAREN expr RPAREN SEMI
        {
            $$ = createNode(NODE_WRITE);
            addChild($$, $3);
        }
    | PRINTF LPAREN STRING RPAREN SEMI
        {
            $$ = createNode(NODE_WRITE);
        }
    | PRINTF LPAREN STRING COMMA expr RPAREN SEMI
        {
            $$ = createNode(NODE_WRITE);
            addChild($$, $5);
        }
    ;

return_stmt
    : RETURN SEMI
        {
            $$ = createNode(NODE_RETURN_STMT);
        }
    | RETURN expr SEMI
        {
            $$ = createNode(NODE_RETURN_STMT);
            addChild($$, $2);
        }
    ;

func_call_stmt
    : ID LPAREN RPAREN SEMI
        {
            if (!isFunctionDefined(symTable, $1)) {
                fprintf(stderr, "Error at line %d: Function '%s' not defined\n", lineNum, $1);
            }
            $$ = createNode(NODE_CALL);
            ASTNode *idNode = createIdNode($1);
            addChild($$, idNode);
        }
    | ID LPAREN arg_list RPAREN SEMI
        {
            if (!isFunctionDefined(symTable, $1)) {
                fprintf(stderr, "Error at line %d: Function '%s' not defined\n", lineNum, $1);
            }
            $$ = createNode(NODE_CALL);
            ASTNode *idNode = createIdNode($1);
            addChild($$, idNode);
            addChild($$, $3);
        }
    ;

arg_list
    : expr
        {
            $$ = createNode(NODE_ARG_LIST);
            addChild($$, $1);
        }
    | arg_list COMMA expr
        {
            addChild($1, $3);
            $$ = $1;
        }
    ;

expr
    : logical_or_expr { $$ = $1; }
    ;

logical_or_expr
    : logical_and_expr { $$ = $1; }
    | logical_or_expr OR logical_and_expr
        {
            $$ = createBinOpNode(OP_OR, $1, $3);
        }
    ;

logical_and_expr
    : comparison_expr { $$ = $1; }
    | logical_and_expr AND comparison_expr
        {
            $$ = createBinOpNode(OP_AND, $1, $3);
        }
    ;

comparison_expr
    : additive_expr { $$ = $1; }
    | comparison_expr EQ additive_expr
        {
            $$ = createBinOpNode(OP_EQ, $1, $3);
        }
    | comparison_expr NE additive_expr
        {
            $$ = createBinOpNode(OP_NE, $1, $3);
        }
    | comparison_expr LT additive_expr
        {
            $$ = createBinOpNode(OP_LT, $1, $3);
        }
    | comparison_expr GT additive_expr
        {
            $$ = createBinOpNode(OP_GT, $1, $3);
        }
    | comparison_expr LE additive_expr
        {
            $$ = createBinOpNode(OP_LE, $1, $3);
        }
    | comparison_expr GE additive_expr
        {
            $$ = createBinOpNode(OP_GE, $1, $3);
        }
    ;

additive_expr
    : multiplicative_expr { $$ = $1; }
    | additive_expr PLUS multiplicative_expr
        {
            $$ = createBinOpNode(OP_PLUS, $1, $3);
        }
    | additive_expr MINUS multiplicative_expr
        {
            $$ = createBinOpNode(OP_MINUS, $1, $3);
        }
    ;

multiplicative_expr
    : unary_expr { $$ = $1; }
    | multiplicative_expr MULT unary_expr
        {
            $$ = createBinOpNode(OP_MULT, $1, $3);
        }
    | multiplicative_expr DIV unary_expr
        {
            $$ = createBinOpNode(OP_DIV, $1, $3);
        }
    | multiplicative_expr MOD unary_expr
        {
            $$ = createBinOpNode(OP_MOD, $1, $3);
        }
    | multiplicative_expr POW unary_expr
        {
            $$ = createBinOpNode(OP_POW, $1, $3);
        }
    ;

unary_expr
    : primary_expr { $$ = $1; }
    | NOT unary_expr
        {
            $$ = createUnOpNode(OP_NOT, $2);
        }
    | MINUS unary_expr %prec UMINUS
        {
            $$ = createUnOpNode(OP_MINUS, $2);
        }
    ;

primary_expr
    : NUM
        {
            $$ = createNumNode($1);
        }
    | ID
        {
            if (!isSymbolDefined(symTable, $1) && !isFunctionDefined(symTable, $1)) {
                fprintf(stderr, "Error at line %d: Variable '%s' not declared\n", lineNum, $1);
            }
            $$ = createIdNode($1);
        }
    | ID LPAREN RPAREN
        {
            if (!isFunctionDefined(symTable, $1)) {
                fprintf(stderr, "Error at line %d: Function '%s' not defined\n", lineNum, $1);
            }
            $$ = createNode(NODE_CALL);
            ASTNode *idNode = createIdNode($1);
            addChild($$, idNode);
        }
    | ID LPAREN arg_list RPAREN
        {
            if (!isFunctionDefined(symTable, $1)) {
                fprintf(stderr, "Error at line %d: Function '%s' not defined\n", lineNum, $1);
            }
            $$ = createNode(NODE_CALL);
            ASTNode *idNode = createIdNode($1);
            addChild($$, idNode);
            addChild($$, $3);
        }
    | LPAREN expr RPAREN
        {
            $$ = $2;
        }
    ;

%%

int yyerror(const char *msg) {
    fprintf(stderr, "Syntax Error at line %d: %s\n", lineNum, msg);
    return 0;
}

