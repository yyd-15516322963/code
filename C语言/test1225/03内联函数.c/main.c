#include <stdio.h>
#include "myhead.h"

extern int max(int a, int b);
int main(int argc, char const *argv[])
{

    printf("%d \n", max(56, 85));
    printf("%d \n", max(83, 58));
    printf("%d \n", max(44, 33));
    return 0;
}
