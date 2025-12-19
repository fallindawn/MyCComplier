CXX = gcc
LEX = flex
YACC = bison
CFLAGS = -I./include -g -Wall
LDFLAGS = -lm

TARGET = compiler
SRCDIR = src
INCDIR = include
OBJDIR = obj

SOURCES = $(wildcard $(SRCDIR)/*.c)
HEADERS = $(wildcard $(INCDIR)/*.h)
LEXER_SRC = $(SRCDIR)/lexer.l
PARSER_SRC = $(SRCDIR)/parser.y

LEX_C = $(SRCDIR)/lex.yy.c
YACC_C = $(SRCDIR)/y.tab.c
YACC_H = $(SRCDIR)/y.tab.h

OBJECTS = $(OBJDIR)/ast.o $(OBJDIR)/symbol_table.o $(OBJDIR)/codegen.o \
          $(OBJDIR)/lex.yy.o $(OBJDIR)/y.tab.o $(OBJDIR)/main.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR):
	@if not exist $(OBJDIR) mkdir $(OBJDIR)

$(OBJDIR)/lex.yy.o: $(LEX_C) $(YACC_H) | $(OBJDIR)
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJDIR)/y.tab.o: $(YACC_C) | $(OBJDIR)
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS) | $(OBJDIR)
	$(CXX) $(CFLAGS) -c $< -o $@

$(LEX_C): $(LEXER_SRC) $(YACC_H)
	$(LEX) -o $@ $<

$(YACC_C) $(YACC_H): $(PARSER_SRC)
	$(YACC) -d -o $(YACC_C) $<

clean:
	@if exist obj rmdir /S /Q obj
	@if exist compiler.exe del compiler.exe
	@if exist compiler del compiler
	@if exist src\lex.yy.c del src\lex.yy.c
	@if exist src\y.tab.c del src\y.tab.c
	@if exist src\y.tab.h del src\y.tab.h
	@if exist *.o del *.o
	@if exist *.exe del *.exe

