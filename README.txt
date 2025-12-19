========================================================================
                    C语言编译器实现报告
========================================================================

1. 项目概述
--------

本项目是编译原理实践的大作业，旨在实现一个简单的C语言编译器。
该编译器能够识别基础的C语言语法，并进行一下操作

- 词法分析
- 语法分析
- 类型检查
- 代码优化
- 错误分析
- 汇编程序

2. 参考资源
--------

本项目参考了以下开源资源和教科书内容：
- 《龙书》(Compilers: Principles, Techniques, and Tools) - Alfred V. Aho et al.
- flex https://blog.csdn.net/wp1603710463/article/details/50365495
- bison https://blog.csdn.net/wp1603710463/article/details/50365640
- flex和bison联合编译 https://blog.csdn.net/weixin_44007632/article/details/108666375
- Makefile https://www.cnblogs.com/rednodel/p/4500388.html
- c代码参考 https://github.com/hopexn/SimpleClangCompiler

本项目与参考代码的主要区别：
- 所有记录都有追踪其行号
- 符号表管理系统
- 自定义的AST（抽象语法树）数据结构，能通过 “结构体 + 函数指针” 模拟面向对象
- 支持完整的表达式处理
- 引入中间代码抽象层，而非直接从语法树生成目标代码，提升了编译器的可扩展性
- 函数递归调用支持
- 错误提示追踪优化
- 模块化，职责分离

3. 编译器功能
--------

3.1 支持的语言特性：

【数据类型】
- 基本整数类型：int, short, long, long long
- 浮点类型：float, double
- 字符类型：char
- 布尔类型：bool
- 空类型：void
- 无符号类型：unsigned int, unsigned short, unsigned long, unsigned long long, unsigned char

【声明】
- 基本声明：int var_name;
- 各类型声明：float x; double y; char ch; bool flag; 等
- 无符号类型声明：unsigned int u_a; unsigned char u_ch; 等
- 函数声明: int func_name(param_list);

【字面量】
- 整数字面量：100, 200, -50
- 浮点字面量：3.14, 2.71828, -1.5
- 字符字面量：'A', 'B', '0'
- 字符串字面量："hello"

【语句】
- 赋值语句：var = expr;
- 输入语句：scanf("%d", &var);
- 输出语句：printf(expr);
- 条件语句：if (condition) {...} 和 if (condition) {...} else {...}
- 循环语句：while (condition) {...}
- 循环语句：for (init; condition; update) {...}
- 复合语句：{...}

【运算符】
- 算术运算：+ - * / % ^
- 关系运算：== != < > <= >=
- 逻辑运算：&& || !

3.2 主要功能模块：

【词法分析 (Lexical Analysis)】
- 使用 Flex 工具完成
- 关键字识别
- 标识符识别
- 数字字面值识别
- 运算符识别
- 注释处理
- 行号追踪
- Token 记录和输出

【语法分析 (Syntax Analysis)】
- 使用 Yacc 工具完成
- 进行语义检查（变量声明检查）
- 语句列表解析
- 赋值/条件/循环语句解析
- 表达式解析（含优先级）
- 构建抽象语法树 (AST)

【抽象语法树 (Abstract Syntax Tree)】
- 自定义的树形数据结构
- 支持各种语言结构的节点类型
- 支持树的遍历和打印

【符号表 (Symbol Table)】
- 存储变量的声明信息
- 支持变量查询和重复定义检查
- 记录变量的类型和声明行号

【中间代码生成 (Intermediate Code Generation)】
- 生成四元式形式的中间代码
- 支持表达式求值
- 支持基本的代码优化框架

【汇编程序 (Assembly Generation)】
- 生成目标汇编代码
- 支持寄存器分配和指令选择
- 支持基本的代码优化

4. 编译系统要求
--------

4.1 依赖软件：
- GCC (GNU C Compiler)
- Flex (Lex 实现)
- Bison (Yacc 实现)
- GNU Make

4.2 在 Windows 上安装依赖：

- 安装 mingw32，包含 GCC、Flex、Bison

5. 编译和运行步骤
--------

5.1 编译编译器：

# 进入项目目录
cd MyCComplier

# 清理旧的构建文件
mingw32-make clean

# 编译项目
mingw32-make

# 成功编译后会生成 compiler 可执行文件

5.2 运行编译器：

# 编译 C 源文件
./compiler test/test1.c

# 编译不同的测试文件
./compiler test/对应文件

6. 编译结果说明
--------

编译器在成功编译后会输出：

6.1 词法分析结果 (Lexical Analysis Results)
- 显示所有识别的 token
- 包括 token 类型、字面值和行号

6.2 抽象语法树 (Abstract Syntax Tree)
- 显示程序的语法树结构
- 用缩进显示树的层级关系
- 显示节点类型和相关信息（如操作符、数值等）

6.3 符号表 (Symbol Table)
- 列出所有声明的变量
- 包括变量名、类型和声明行号

6.4 中间代码 (Intermediate Code)
- 生成的四元式序列
- 每个四元式包括：操作符、两个操作数和结果

6.5 汇编代码 (Assembly Code)
- 生成的汇编指令序列
- 包括数据段和代码段

7. 测试用例
--------

项目包含 5 个测试文件：

test1.c - 基本的变量声明和赋值
test2.c - 循环和条件语句
test3.c - 各种算术运算
test4.c - 逻辑运算
test5.c - for 循环语句
test_func.c -函数定义和调用

8. 实现细节
--------

8.1 词法分析器 (lexer.l)
- 使用 Flex 实现
- 支持多行注释 /* */ 和单行注释 //
- 自动跟踪行号
- 记录每个 token 的信息以供后续阶段使用

8.2 语法分析器 (parser.y)
- 使用 Bison (Yacc) 实现
- LALR(1) 分析算法
- 操作符优先级和结合性定义
- 错误恢复机制
- 构建完整的 AST

8.3 AST 实现 (ast.c)
- 动态内存分配的树结构
- 支持任意数量的子节点
- 包含节点类型和数据信息
- 提供树的遍历和释放函数

8.4 符号表实现 (symbol_table.c)
- 线性搜索的符号表
- 支持作用域管理框架
- 重复定义检查
- 类型信息存储

8.5 中间代码生成 (codegen.c)
- 四元式格式的中间代码
- 临时变量自动管理 (t0, t1, t2, ...)
- 标签生成 (L0, L1, L2, ...)
- 支持表达式的递归下降翻译

9. 错误处理
--------

编译器能够检测和报告以下错误：

- 未声明的变量使用
- 变量属性错误
- 语法错误
- 文件打开失败
- 语法树构建失败

错误信息格式：
```
Error at line X: Description of error
Warning: Something that might be wrong
```

11. 已知限制
--------

当前实现的限制：

- 不支持数组和指针
- 中间代码生成较为基础
- 符号表未实现作用域隔离

12. 故障排除
--------

【问题】在初始实现中，生成的AST树中，函数定义和return语句显示为"UNKNOWN"。
【原因分析】
- `printAST()` 和 `printASTToFile()` 函数缺少 NODE_FUNC_DEF 和 NODE_RETURN_STMT 的处理case
- 导致这些节点类型无法正确转换为字符串显示
【解决】
在 `printAST()` 函数中添加特殊处理：
case NODE_FUNC_DEF:
    printf("%*sFUNC_DEF [function]\n", indent, "");
    // 打印子节点
    for (int i = 0; i < node->childrenCount; i++) {
        printAST(node->children[i], indent + 2);
    }
    break;

case NODE_CALL:
    printf("%*sCALL [call]\n", indent, "");
    // 打印子节点
    for (int i = 0; i < node->childrenCount; i++) {
        printAST(node->children[i], indent + 2);
    }
    break;

case NODE_RETURN_STMT:
    printf("%*sRETURN\n", indent, "");
    // 打印子节点
    for (int i = 0; i < node->childrenCount; i++) {
        printAST(node->children[i], indent + 2);
    }
    break;

【问题】函数调用代码生成修复
在初始实现中，中间代码中完全没有函数调用的PARAM和CALL指令，只有简单的赋值操作。
【原因分析】
- `generateIntermediateCodeExpr()` 函数中缺少对函数调用节点 (NODE_CALL) 的处理逻辑
【解决】
在 `generateIntermediateCodeExpr()` 函数中添加对 NODE_CALL 的处理：
case NODE_CALL: {
    if (ast->childrenCount > 0) {
        ASTNode *funcName = ast->children[0];
        
        if (funcName->type == NODE_ID) {
            /* 处理函数参数 */
            if (ast->childrenCount > 1 && ast->children[1]->type == NODE_ARG_LIST) {
                ASTNode *argList = ast->children[1];
                
                /* 为每个参数生成PARAM指令 */
                for (int i = 0; i < argList->childrenCount; i++) {
                    char *argTemp = getTempVar(gen);
                    generateIntermediateCodeExpr(argList->children[i], gen, table, argTemp);
                    genCode(gen, "PARAM", argTemp, "", "");
                }
            }
            
            /* 生成CALL指令 */
            sprintf(funcCall, "CALL %s", funcName->name);
            genCode(gen, funcCall, "", "", result);
        }
    }
    break;
}

【问题】函数定义部分（add、subtract等）的代码没有生成，所有代码都放在全局代码中执行。汇编代码中有 `call add` 但没有 `add:` 标签和函数体.
【原因分析】
`generateIntermediateCode()` 函数中缺少 NODE_FUNC_DEF 的处理case，导致函数定义节点被当作普通语句列表处理。
【解决】
在 `generateIntermediateCode()` 函数中添加对 NODE_FUNC_DEF 的处理：
case NODE_FUNC_DEF: {
    if (ast->childrenCount >= 2) {
        ASTNode *funcName = ast->children[0];
        ASTNode *paramList = NULL;
        ASTNode *stmtList = NULL;
        
        /* 判断是否有参数列表 */
        if (ast->childrenCount == 3) {
            paramList = ast->children[1];
            stmtList = ast->children[2];
        } else {
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


【问题】中间代码中临时变量被重复赋值，导致计算错误
【原因分析】
`getTempVar()` 函数实现问题：

char* getTempVar(CodeGenerator *gen) {
    static char buf[32];  // 静态缓冲区
    sprintf(buf, "t%d", gen->tempVarCount++);
    return buf;  // 返回同一个指针


问题在于：
- 函数返回指向 `static char buf` 的指针
- 在递归调用中，后续的 `getTempVar()` 会覆盖之前的值
- 虽然 `genCode()` 会立即进行 `strncpy()` 复制，但在某些嵌套调用中仍然会出现问题

【解决】
/* NODE_BINOP 修复 */
case NODE_BINOP: {
    if (ast->childrenCount >= 2) {
        char temp1_buf[32], temp2_buf[32];
        temp1 = getTempVar(gen);
        strcpy(temp1_buf, temp1);  // 立即复制到本地缓冲区
        temp2 = getTempVar(gen);
        strcpy(temp2_buf, temp2);  // 立即复制到本地缓冲区
        generateIntermediateCodeExpr(ast->children[0], gen, table, temp1_buf);
        generateIntermediateCodeExpr(ast->children[1], gen, table, temp2_buf);
        genCode(gen, getOpString(ast->op), temp1_buf, temp2_buf, result);
    }
    break;
}

/* NODE_UNOP 修复 */
case NODE_UNOP:
    if (ast->childrenCount > 0) {
        char temp1_buf[32];
        temp1 = getTempVar(gen);
        strcpy(temp1_buf, temp1);
        generateIntermediateCodeExpr(ast->children[0], gen, table, temp1_buf);
        genCode(gen, getOpString(ast->op), temp1_buf, "", result);
    }
    break;

/* NODE_WRITE 修复 */
case NODE_WRITE:
    if (ast->childrenCount > 0) {
        char tempVar_buf[32];
        char *tempVar = getTempVar(gen);
        strcpy(tempVar_buf, tempVar);
        generateIntermediateCodeExpr(ast->children[0], gen, table, tempVar_buf);
        genCode(gen, "WRITE", tempVar_buf, "", "");
    }
    break;

/* NODE_RETURN_STMT 修复 */
case NODE_RETURN_STMT: {
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

/* NODE_CALL 修复 */
for (int i = 0; i < argList->childrenCount; i++) {
    char argTemp_buf[32];
    char *argTemp = getTempVar(gen);
    strcpy(argTemp_buf, argTemp);  // 立即复制
    generateIntermediateCodeExpr(argList->children[i], gen, table, argTemp_buf);
    genCode(gen, "PARAM", argTemp_buf, "", "");
}
```

**修复的核心思路：**
1. 调用 `getTempVar()` 获取临时变量名
2. 立即用 `strcpy()` 复制到本地缓冲区
3. 使用本地缓冲区作为参数，避免 static 缓冲区被覆盖
========================================================================
