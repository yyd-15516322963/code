#include <stdio.h>

int main()
{
    // 中间变量法
    int a = 2, b = 4, temp;
    temp = a;
    a = b;
    b = temp;
    printf("%d %d\n", a, b);
    // 加减法
    a = a + b;
    b = a - b;
    a = a - b;
    printf("%d %d\n", a, b);
    // 异或法
    a = a ^ b;
    b = a ^ b;
    a = a ^ b;
    printf("%d %d\n", a, b);
}