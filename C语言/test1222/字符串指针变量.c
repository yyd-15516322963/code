#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    // s是一个指针变量,存储字符串常量"hello"首元素地址
    char *s = "hello world";

    // 通过指针(地址)访问hello
    // 求字符串长度
    printf("%ld \n", strlen(s));
    // 打印字符串
    printf("%s \n", s);
    // 指针就是地址,所以我们可以通过地址的运算访问字符串中指定的字符
    printf("%c \n", *s);
    printf("%c \n", s[0]);

    printf("%s \n", s + 1); // 指针s指向char类型,加减运算按照char类型大小进行偏移
    printf("%ld \n", strlen(s + 1));
    printf("%c \n", *(s + 1));
    printf("%c \n", s[1]);

    // scanf("%s", s); // Segmentation fault段错误, s指向的内存是只读,无法被修改
    // s[2] = 'L'; // Segmentation fault段错误,常量不可被修改

    // s1是一个指针变量,存储str[] 数组变量 "hello"首元素地址
    char str[] = "hello world!";
    char *s1 = str;

    // 通过指针(地址)访问hello
    // 求字符串长度
    printf("s1 %ld \n", strlen(s1));
    printf("s1 %ld \n", sizeof(s1));
    printf("str %ld \n", strlen(str));
    printf("str %ld \n", sizeof(str));
    // 打印字符串
    printf("s1 %s \n", s1);
    // 指针就是地址,所以我们可以通过地址的运算访问字符串中指定的字符
    printf("s1 %c \n", *s1);
    printf("s1 %c \n", s1[0]);

    printf("s1 %s \n", s1 + 1); // 指针s指向char类型,加减运算按照char类型大小进行偏移
    printf("s1 %ld \n", strlen(s1 + 1));
    printf("s1 %c \n", *(s1 + 1));
    printf("s1 %c \n", s1[1]);

    scanf("%s", s1); // 正确, s1指向的内存是变量str,可以被修改
    printf("s1 %s \n", s1);
    s1[2] = 'L'; // 变量可被修改
    printf("s1 %s \n", s1);
    printf("%s \n",str);



    char str2[14] = "nihao shijie!";

    char (*p1)[14] = &str2; // 不再是首元素地址,而是整个数组本身的地址, 指针类型就是数组本身char [14]类型指针

    return 0;
}
