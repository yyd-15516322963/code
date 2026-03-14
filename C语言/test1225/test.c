#include <stdio.h>
main()
{
    int a, b, k = 4, m = 4, *p1 = &k, *p2 = &m;
    a = p1 == &m;
    b = (*p1) / (*p2) + 7;
    printf("a=%d\n", a);
    printf("b=%d\n", b);
}
