// 2、编写一个程序，清除用户输入字符串中的空格符并将之输出。
// （例如用户输入”a b”，输出”ab”

#include <stdio.h>

int main(int argc, char const *argv[])
{
    char str[99] = {0};
    char cStr[99] = {0};
    int i, j = 0;
    scanf("%98[^\n]s", str);
    for (i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == ' ')
        {   
            continue;
        }
        cStr[j] = str[i];
        j++;
    }
    cStr[j] = '\0';
    printf("%s\n", cStr);
    return 0;

    // char str[99];
    // int i, j = 0;
    // scanf("%98[^\n]s", str);
    // for (i = 0; str[i] != '\0'; i++)
    // {
    //     if (str[i] != ' ')
    //     {
    //         str[j] = str[i];
    //         j++;
    //     }
    // }
    // str[j] = '\0';
    // printf("%s\n", str);
    // return 0;
}
