#include <stdio.h>

int main(int argc, char const *argv[])
{
    // arr1 长度为[3]  子元素类型 char [6]
    char arr1[3][10] = {
        "abc",
        "hello!!!!",
        "world"};
    printf("%ld \n", sizeof(arr1));
    printf("%s \n", arr1[1]);
    printf("%c \n", arr1[1][1]);
    // arr2长度为[3], 子元素类型char *类型
    // arr2是一个一维数组
    char *arr2[3] = {
        "abc",
        "hello!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",
        "world"};
    printf("%ld \n", sizeof(arr2));
    printf("%s \n", arr2[1]);
    printf("%c \n", *(arr2[1] + 1));
    printf("%c \n", arr2[1][1]);
    // 等价与下者
    char *p1 = "abc";
    char *p2 = "hello";
    char *p3 = "world";
    char *arr3[3] = {p1, p2, p3};

    return 0;
}
