/* 测试代码优化功能 */

int add(int a, int b) {
    int result;
    result = a + b;
    return result;
}

int main() {
    int x;
    int y;
    int z;
    int sum;
    int temp;
    
    x = 10;
    y = 5;
    z = 0;

    /* 常数折叠测试 */
    sum = add(x + 5, y * 2);  /* x + 5 = 15, y * 2 = 10 */

    /* 代数简化测试 */
    z = sum + 0;    /* 应该简化为 z = sum */
    z = z * 1;      /* 应该简化为 z = z */

    /* 死代码消除测试 */
    temp = x + y;  /* 这个temp如果不使用，应该被消除 */

    return z;
}
