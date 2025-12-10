%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "ast.h"
#include "y.tab.h"

int lineNum = 1;
int tokenCount = 0;

TokenRecord tokenRecords[10000];
int recordCount = 0;

void recordToken(const char *token, const char *lexeme) {
    if (recordCount < 10000) {
        strcpy(tokenRecords[recordCount].token, token);
        strcpy(tokenRecords[recordCount].lexeme, lexeme);
        tokenRecords[recordCount].lineNum = lineNum;
        recordCount++;
    }
}

void printTokens() {
    printf("\n========== Lexical Analysis Results ==========\n");
    printf("%-5s %-20s %-20s %-10s\n", "No.", "Token Type", "Lexeme", "Line");
    printf("----------------------------------------------\n");
    for (int i = 0; i < recordCount; i++) {
        printf("%-5d %-20s %-20s %-10d\n", i+1, tokenRecords[i].token, 
               tokenRecords[i].lexeme, tokenRecords[i].lineNum);
    }
    printf("==============================================\n\n");
}

%}

%%

"/*".*"*/"      { /* 注释 */ }
"//".*          { /* 注释 */ }

"int"           { recordToken("KEYWORD_INT", yytext); return INT; }
"if"            { recordToken("KEYWORD_IF", yytext); return IF; }
"else"          { recordToken("KEYWORD_ELSE", yytext); return ELSE; }
"while"         { recordToken("KEYWORD_WHILE", yytext); return WHILE; }
"for"           { recordToken("KEYWORD_FOR", yytext); return FOR; }
"scanf"         { recordToken("KEYWORD_SCANF", yytext); return SCANF; }
"printf"        { recordToken("KEYWORD_PRINTF", yytext); return PRINTF; }

"+"             { recordToken("OP_PLUS", yytext); return PLUS; }
"-"             { recordToken("OP_MINUS", yytext); return MINUS; }
"*"             { recordToken("OP_MULT", yytext); return MULT; }
"/"             { recordToken("OP_DIV", yytext); return DIV; }
"%"             { recordToken("OP_MOD", yytext); return MOD; }
"^"             { recordToken("OP_POW", yytext); return POW; }

"=="            { recordToken("OP_EQ", yytext); return EQ; }
"!="            { recordToken("OP_NE", yytext); return NE; }
"<"             { recordToken("OP_LT", yytext); return LT; }
">"             { recordToken("OP_GT", yytext); return GT; }
"<="            { recordToken("OP_LE", yytext); return LE; }
">="            { recordToken("OP_GE", yytext); return GE; }

"&&"            { recordToken("OP_AND", yytext); return AND; }
"||"            { recordToken("OP_OR", yytext); return OR; }
"!"             { recordToken("OP_NOT", yytext); return NOT; }

"="             { recordToken("OP_ASSIGN", yytext); return ASSIGN; }

"("             { recordToken("LPAREN", yytext); return LPAREN; }
")"             { recordToken("RPAREN", yytext); return RPAREN; }
"{"             { recordToken("LBRACE", yytext); return LBRACE; }
"}"             { recordToken("RBRACE", yytext); return RBRACE; }
";"             { recordToken("SEMI", yytext); return SEMI; }
","             { recordToken("COMMA", yytext); return COMMA; }

[a-zA-Z_][a-zA-Z0-9_]* {
    recordToken("IDENTIFIER", yytext);
    yylval.id = (char *)malloc(strlen(yytext) + 1);
    strcpy(yylval.id, yytext);
    return ID;
}

[0-9]+          {
    recordToken("NUMBER", yytext);
    yylval.num = atoi(yytext);
    return NUM;
}

\"[^\"]*\"      {
    recordToken("STRING", yytext);
    yylval.str = (char *)malloc(strlen(yytext) + 1);
    strcpy(yylval.str, yytext);
    return STRING;
}

\n              { lineNum++; }
[ \t]           { /* 空白 */ }
.               { printf("Warning: Unknown character: %s\n", yytext); }

%%

int yywrap() {
    return 1;
}

