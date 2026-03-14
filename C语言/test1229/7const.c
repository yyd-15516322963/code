#include <stdio.h>

int main(int argc, char const *argv[])
{
    int a = 10, b = 11, c = 12;
    const int *p = &a;

    p = &b;
    printf("%d \n", *p);
    
    // *p = 20;  指针指向地址的值不可改
    int *const q = &b;

    // q = &c;  指针指向的地址不可改

    *q = 20;

    const int *const p1 = &c;

    // p1 = &b;
    // *p1 = 20;   指针指向的地址和指向地址的值都不可改

    return 0;
}
