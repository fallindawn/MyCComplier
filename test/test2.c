/* 验证循环和分支语句 */
int main() {
    int a;
    int i;

    a = 0;
    i = 0;

    while (i < 10) {
        a = a + i;
        i = i + 1;
    }

    if (a > 30) {
        printf("%d\n", a);
    }
    return 0;
}