#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    char str1[100] = {"hello world"};
    char cStr2[25] = {"hello World"};
    int lenth = 0;
    // int len = strlen(str1);原理
    while (str1[lenth] != '\0')
    {
        lenth++;
    }
    printf("%d\n", lenth);
    return 0;
}
