#include <stdio.h>

int main(int argc, char const *argv[])
{
    int a = 100;
    int *p = &a;

    int **q = &p;
    int **qq = p;//错误

    printf("%p %p \n", q, &p);
    printf("%p %p \n", qq, p);
    return 0;
}
