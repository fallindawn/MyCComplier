/* 测试函数定义和调用 - 避免变量重名 */

int add(int p, int q) {
    int res;
    res = p + q;
    return res;
}

int subtract(int u, int v) {
    return u - v;
}

int main() {
    int a;
    int b;
    int sum;
    int diff;
    
    a = 100;
    b = 50;
    
    sum = add(a, b);
    diff = subtract(a, b);
    
    printf("%d\n", sum);
    printf("%d\n", diff);
    
    return 0;
}
