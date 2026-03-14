#include <stdio.h>

int change(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
    // printf("交换后:%d %d\n", a, b);
}
int main(int argc, char const *argv[])
{
    int x, y;
    scanf("%d %d", &x, &y);
    printf("交换前:%d %d\n", x, y);
    
    change(&x, &y);
    printf("交换后:%d %d\n", x, y);
    return 0;
}
