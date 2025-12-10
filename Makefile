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

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS) | $(OBJDIR)
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJDIR)/lex.yy.o: $(LEX_C) | $(OBJDIR)
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJDIR)/y.tab.o: $(YACC_C) | $(OBJDIR)
	$(CXX) $(CFLAGS) -c $< -o $@

$(LEX_C): $(LEXER_SRC)
	$(LEX) -o $@ $<

$(YACC_C) $(YACC_H): $(PARSER_SRC)
	$(YACC) -d -o $(YACC_C) $<

clean:
	@if exist $(OBJDIR) rmdir /S /Q $(OBJDIR)
	@if exist $(TARGET).exe del /Q $(TARGET).exe
	@if exist $(TARGET) del /Q $(TARGET)
	@if exist $(LEX_C) del /Q $(LEX_C)
	@if exist $(YACC_C) del /Q $(YACC_C)
	@if exist $(YACC_H) del /Q $(YACC_H)
	@if exist *.o del /Q *.o

