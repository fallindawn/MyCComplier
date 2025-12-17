CXX = gcc                     	# 编译器，使用gcc（用于C语言）
LEX = flex						# 词法分析器生成器
YACC = bison					# 语法分析器生成器
CFLAGS = -I./include -g -Wall	# 编译选项：包含头文件路径、调试信息、显示所有警告
LDFLAGS = -lm					# 链接选项：链接数学库

TARGET = compiler				# 最终生成的可执行文件名
SRCDIR = src					# 源代码目录
INCDIR = include				# 头文件目录
OBJDIR = obj					# 目标文件目录

# 获取所有.c和.h文件
SOURCES = $(wildcard $(SRCDIR)/*.c)
HEADERS = $(wildcard $(INCDIR)/*.h)

# 词法和语法规则文件
LEXER_SRC = $(SRCDIR)/lexer.l
PARSER_SRC = $(SRCDIR)/parser.y

# flex/bison生成的文件
LEX_C = $(SRCDIR)/lex.yy.c
YACC_C = $(SRCDIR)/y.tab.c
YACC_H = $(SRCDIR)/y.tab.h

# 需要编译的所有目标文件列表
OBJECTS = $(OBJDIR)/ast.o $(OBJDIR)/symbol_table.o $(OBJDIR)/codegen.o \
          $(OBJDIR)/lex.yy.o $(OBJDIR)/y.tab.o $(OBJDIR)/main.o

.PHONY: all clean  				# 声明all和clean是伪目标，不与实际文件关联

all: $(TARGET)					# 默认目标：构建编译器

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

#清理规则
clean:
	@if exist $(OBJDIR) rmdir /S /Q $(OBJDIR)
	@if exist $(TARGET).exe del /Q $(TARGET).exe
	@if exist $(TARGET) del /Q $(TARGET)
	@if exist $(LEX_C) del /Q $(LEX_C)
	@if exist $(YACC_C) del /Q $(YACC_C)
	@if exist $(YACC_H) del /Q $(YACC_H)
	@if exist *.o del /Q *.o

