#include <stdio.h>
#define PI 3.14

#define ADD(a, b) a + b
// 要加括号，否则运算结果可能会出错，因为只是简单的文本替换
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int main()
{
    printf("%d \n", ADD(10, 4));
    printf("%f \n", PI);
    // 如果宏定义 不加括号，这里返回结果为0
    printf("%d \n", MAX(10, 0 > 9));
}