/* 验证多种类型声明的测试 */

/* 基本整数类型 */
int main(){
    int a;
    short b;
    long c;
    long long d;

    /* 浮点类型 */
    float x;
    double y;

    /* 字符类型 */
    char ch;

    /* 无符号整数类型 */
    unsigned int u_a;
    unsigned char u_ch;
    unsigned short u_b;
    unsigned long u_c;

    /* 布尔类型 */
    bool flag;

    /* 空类型 */
    void func;

    /* 初始化和使用 */
    a = 100;
    x = 3.14;
    y = 2.71828;
    ch = 'A';

    /* 表达式 */
    c = a + a;
    y = x * y;
    return 0;
}