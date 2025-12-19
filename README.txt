========================================================================
                    C语言编译器实现报告
========================================================================

1. 项目概述
--------

本项目是编译原理实践的大作业，旨在实现一个简单的C语言编译器。
该编译器能够识别基础的C语言语法，并进行一下操作

- [x] 词法分析
- [x] 语法分析
- [x] 类型检查
- [x] 代码优化
- [x] 错误分析
- [x] 汇编程序

2. 参考资源
--------

本项目参考了以下开源资源和教科书内容：
- 《龙书》(Compilers: Principles, Techniques, and Tools) - Alfred V. Aho et al.
- flex https://blog.csdn.net/wp1603710463/article/details/50365495
- bison https://blog.csdn.net/wp1603710463/article/details/50365640
- flex和bison联合编译 https://blog.csdn.net/weixin_44007632/article/details/108666375
- Makefile https://www.cnblogs.com/rednodel/p/4500388.html
- 代码参考 https://github.com/hopexn/SimpleClangCompiler

本项目与参考代码的主要区别：
- 完全自主实现了符号表管理系统
- 自定义的AST（抽象语法树）数据结构
- 独立开发的四元式中间代码生成器
- 自主实现了词法分析的token记录和打印功能
- 支持完整的表达式处理（二元运算、一元运算、优先级等）
- 完整的语句结构支持（if-else、while、for等）

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
- 关键字识别（int, if, else, while, for, scanf, printf）
- 标识符识别
- 数字字面值识别
- 运算符识别
- 注释处理（/* */ 和 //）
- 行号追踪
- Token 记录和输出

【语法分析 (Syntax Analysis)】
- 使用 Yacc 工具完成
- 采用 LALR(1) 分析算法
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

4. 编译系统要求
--------

4.1 依赖软件：
- GCC (GNU C Compiler)
- Flex (Lex 实现)
- Bison (Yacc 实现) 或 yacc
- GNU Make
- 标准 C 库

4.2 在 Windows 上安装依赖：

方法 1：使用 mingw32
- 安装 mingw32，包含 GCC、Flex、Bison

方法 2：使用 WSL (Windows Subsystem for Linux)
```
# 在 WSL 中
sudo apt update
sudo apt install build-essential lex yacc
```

5. 编译和运行步骤
--------

5.1 编译编译器：

```bash
# 进入项目目录
cd MyCComplier

# 清理旧的构建文件
mingw32-make clean

# 编译项目
mingw32-make

# 成功编译后会生成 compiler 可执行文件
```

5.2 运行编译器：

```bash
# 编译 C 源文件
./compiler test/test1.c

# 编译不同的测试文件
./compiler test/test2.c
./compiler test/test3.c
./compiler test/test4.c
./compiler test/test5.c
./compiler test/test_types.c
```

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
- 示例输出：

7. 测试用例
--------

项目包含 5 个测试文件：

test1.c - 基本的变量声明和赋值
test2.c - 循环和条件语句
test3.c - 各种算术运算
test4.c - 逻辑运算
test5.c - for 循环语句
test_types.c -新增各种变量声明、赋值

运行所有测试：
```bash
for test in test/*.c; do
    echo "========== Testing $test =========="
    ./compiler "$test"
    echo ""
done
```

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
- 语法错误（通过 Yacc 的错误恢复）
- 文件打开失败
- 语法树构建失败

错误信息格式：
```
Error at line X: Description of error
Warning: Something that might be wrong
```

10. 扩展功能建议
--------

以下功能可以进一步实现：

【代码优化】
- 常量折叠
- 死代码消除
- 公共子表达式消除
- 变量传播优化

【目标代码生成】
- x86 汇编代码生成
- 寄存器分配
- 指令选择优化

【前端改进】
- 浮点数支持
- 数组支持
- 函数定义和调用
- 结构体和指针
- 更多控制流语句

【运行时环境】
- 虚拟机执行器
- 栈帧管理
- 内存管理

11. 已知限制
--------

当前实现的限制：

- 不支持数组和指针
- 不支持函数定义（仅支持调用 scanf/printf）
- 中间代码生成较为基础
- 没有对代码进行优化
- 符号表未实现作用域隔离

12. 故障排除
--------

【问题】编译时出现 "command not found: flex" 或 "command not found: bison"
【解决】安装相应的工具包（见第 4 节）

【问题】出现 undefined reference 错误
【解决】确保所有 .c 文件都被包含在编译中（检查 Makefile）

【问题】生成的可执行文件无法运行
【解决】检查文件路径、权限和依赖库
========================================================================
