// 4、编写一个程序，将两个字符串连接起来，不要用 strcat 或 strncat 函数。
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    char str1[99] = "abcd";
    char str2[] = "efgh";
    int i = 0, j = 0;
    int len = strlen(str1);
    int len1 = strlen(str2);
    while (i < len)
    {
        i++;
    }
    while (j < len1)
    {
        str1[i] = str2[j];
        i++;
        j++;
    }
    // int len = strlen(str1);
    // int len1 = strlen(str2);
    // for(int i = 0;i < strlen(str2);i++)
    // {
    //     str1[len + i] = str2[i];
    // }
    str1[len + len1] = '\0';
    printf("%s\n",str1);
    return 0;
}
