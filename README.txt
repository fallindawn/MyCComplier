========================================================================
                    C语言编译器实现报告
========================================================================

1. 项目概述
--------

本项目是编译原理实践的大作业，旨在实现一个简单的C语言编译器。该编译器能够
识别基础的C语言语法，进行词法分析、语法分析、构建语法树，以及生成中间代码。

2. 参考资源
--------

本项目参考了以下开源资源和教科书内容：
- 《龙书》(Compilers: Principles, Techniques, and Tools) - Alfred V. Aho et al.
- Lex/Yacc 标准文档 - https://en.wikipedia.org/wiki/Lex_(software)
- 编译器设计相关教学资料

本项目与参考代码的主要区别：
✓ 完全自主实现了符号表管理系统
✓ 自定义的AST（抽象语法树）数据结构
✓ 独立开发的四元式中间代码生成器
✓ 自主实现了词法分析的token记录和打印功能
✓ 支持完整的表达式处理（二元运算、一元运算、优先级等）
✓ 完整的语句结构支持（if-else、while、for等）

3. 编译器功能
--------

3.1 支持的语言特性：

【数据类型】
- int：整数类型

【声明】
- 变量声明：int var_name;

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
- 使用 Lex 工具完成
- 识别关键字、标识符、数字、运算符等
- 生成 token 序列
- 记录每个 token 的行号信息

【语法分析 (Syntax Analysis)】
- 使用 Yacc 工具完成
- 采用 LALR(1) 分析算法
- 构建抽象语法树 (AST)
- 进行语义检查（变量声明检查）

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

方法 1：使用 MinGW-w64 或 Cygwin
- 安装 MinGW-w64，包含 GCC、Flex、Bison
- 或使用 Cygwin 安装相同工具

方法 2：使用 MSYS2
```
# 安装 MSYS2
# 然后在 MSYS2 中执行：
pacman -Syu
pacman -S mingw-w64-x86_64-gcc flex bison make
```

方法 3：使用 WSL (Windows Subsystem for Linux)
```
# 在 WSL 中
sudo apt update
sudo apt install build-essential flex bison
```

4.3 在 Linux 上安装依赖：
```
Ubuntu/Debian:
sudo apt update
sudo apt install build-essential flex bison

Fedora/RedHat:
sudo dnf install gcc flex bison make

Arch Linux:
sudo pacman -S gcc flex bison
```

5. 编译和运行步骤
--------

5.1 编译编译器：

```bash
# 进入项目目录
cd myCComplier

# 清理旧的构建文件
make clean

# 编译项目
make

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
```

6. 编译结果说明
--------

编译器在成功编译后会输出：

6.1 词法分析结果 (Lexical Analysis Results)
- 显示所有识别的 token
- 包括 token 类型、字面值和行号
- 示例输出：
  ┌─────────────────────────────────────────┐
  │ No. │ Token Type │ Lexeme │ Line │
  ├─────────────────────────────────────────┤
  │  1  │    INT     │  int   │  1   │
  │  2  │ IDENTIFIER │   x    │  1   │
  │  3  │    SEMI    │   ;    │  1   │
  └─────────────────────────────────────────┘

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
  ┌──────────────────────────────────────────────────┐
  │ Num │ Op │ Arg1 │ Arg2 │ Result │
  ├──────────────────────────────────────────────────┤
  │  0  │ = │  10  │      │   a    │
  │  1  │ = │  20  │      │   b    │
  │  2  │ + │  a   │  b   │   t0   │
  │  3  │ = │  t0  │      │   c    │
  └──────────────────────────────────────────────────┘

7. 测试用例
--------

项目包含 5 个测试文件：

test1.c - 基本的变量声明和赋值
test2.c - 循环和条件语句
test3.c - 各种算术运算
test4.c - 逻辑运算
test5.c - for 循环语句

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

✓ 未声明的变量使用
✓ 语法错误（通过 Yacc 的错误恢复）
✓ 文件打开失败
✓ 语法树构建失败

错误信息格式：
```
Error at line X: Description of error
Warning: Something that might be wrong
```

10. 代码结构
--------

myCComplier/
├── Makefile                 # 编译配置文件
├── include/                 # 头文件目录
│   ├── ast.h               # AST 定义
│   ├── symbol_table.h      # 符号表定义
│   └── codegen.h           # 中间代码生成定义
├── src/                    # 源文件目录
│   ├── main.c              # 主程序
│   ├── ast.c               # AST 实现
│   ├── symbol_table.c      # 符号表实现
│   ├── codegen.c           # 中间代码生成实现
│   ├── lexer.l             # Lex 词法分析器
│   └── parser.y            # Yacc 语法分析器
├── test/                   # 测试用例目录
│   ├── test1.c
│   ├── test2.c
│   ├── test3.c
│   ├── test4.c
│   └── test5.c
└── README.txt             # 本文件

11. 扩展功能建议
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

12. 已知限制
--------

当前实现的限制：

- 仅支持 int 类型
- 不支持数组和指针
- 不支持函数定义（仅支持调用 scanf/printf）
- 中间代码生成较为基础
- 没有对代码进行优化
- 符号表未实现作用域隔离

13. 故障排除
--------

【问题】编译时出现 "command not found: flex" 或 "command not found: bison"
【解决】安装相应的工具包（见第 4 节）

【问题】出现 undefined reference 错误
【解决】确保所有 .c 文件都被包含在编译中（检查 Makefile）

【问题】生成的可执行文件无法运行
【解决】检查文件路径、权限和依赖库

14. 联系方式和支持
--------

项目维护者：[学生姓名]
创建日期：2025 年
最后更新：2025 年

========================================================================
