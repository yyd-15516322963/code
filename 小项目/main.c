#include <stdio.h>
int main(int argc, char const *argv[])
{
    // float a = 100.0;
    // printf("%10f\n", a);
    // int ret = getchar();
    // printf("%d\n", ret);

    // int b = 1, c = 2, d = 3, e = 4;
    // int n = (b + c, d + e);
    // printf("%d\n", n);
    // char arr[] = "hello world";
    // char *p = arr;
    // p[2] = 'L';
    // printf("%s\n",p);

    int a = 10;
    int *p = &a;
    int *q = p;
    printf("%d\n",*q);
    printf("%d\n",*p);

    return 0;
}
