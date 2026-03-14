// 3、将一个字符串倒序 "abcd" --->"dcba"

#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    char str[100] = {0};
    char cStr[100] = {0};

    scanf("%99[^\n]", str);
    int len = strlen(str);
    int i, j = 0;

    for (i = len - 1; i >= 0; i--)
    {
        cStr[j] = str[i];
        j++;
    }
    cStr[j] = '\0';
    printf("%s\n", cStr);
    return 0;
}
