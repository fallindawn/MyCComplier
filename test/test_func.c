/* 测试函数识别和函数调用功能 */

int add(int a, int b) {
    int result;
    result = a + b;
    return result;
}

int subtract(int x, int y) {
    return x - y;
}

float divide(float p, float q) {
    return p / q;
}

int a;
int b;
int c;
int sum;
int diff;

a = 10;
b = 5;
c = 0;

sum = add(a, b);
diff = subtract(a, b);

printf("%d\n", sum);
printf("%d\n", diff);



