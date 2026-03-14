#include <stdio.h>

int main(int argc, char const *argv[])
{
    char *arr[3] = {
        "abc",
        "hello",
        "world"};
    // arr[0],arr[1],arr[2] 都是一个char *类型的指针变量,存着一个char类型地址
    //  指针变量的值是可以修改的
    arr[1] = "demo";

    printf("%s \n", arr[1]);

    int a = 17;
    int *p = &a;

    int b = 55;

    p = &b; // 指针自身的指向变了

    *p = 77;

    char str1[] = "abc";
    char str2[] = "hello";
    char str3[] = "world";

    char *arr2[3] = {
        str1,
        str2,
        str3};

    return 0;
}
