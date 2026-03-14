#include <stdio.h>

int main(int argc, char const *argv[])
{
    char str[] = "hello";
    int num = 7;

    num++;
    str[0] = 'H';
    printf("%s \n", str);

    // "hello" 也是一个匿名数组, 数组常量的首字母地址
    printf("%s \n", "hello");
    // p指针指向 "hello" 首字母地址
    char *p = "hello";
    printf("%s \n", p);
    // 因为 str 数组表示也是数组首元素的地址str[index] 取到指定下标的元素

    printf("%c \n", p[1]);
    printf("%c \n", p[2]);

    printf("%c \n", str[1]);

    // p 指向的是字符常量的地址
    // str 是变量地址
    str[1] = 'E';
    printf("%s \n", str);

    // p不可以通过下标修改
    p[1] = 'E'; // Segmentation fault 段错误


    return 0;
}
