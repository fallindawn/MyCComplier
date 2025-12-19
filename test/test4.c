/* 验证双目运算符测试 */
int main() {
    int a;
    int b;

    a = 1;
    b = 0;

    if (a && b) {
        printf("0");
    }

    if (a || b) {
        printf("1");
    }

    if (!b) {
        printf("1");
    }
    return 0;
}