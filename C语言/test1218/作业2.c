// 4、华为笔试题，输入google,输出 gole（过滤相同的字母）
// char str=[32] = {'\0'};
// scanf("%s", str);  //暂时不考虑空格#include <stdio.h>
// 输出
// 请输入32个字符：google
// gole
#include <stdio.h>
#include <string.h>
int main(int argc, char const *argv[])
{
    char str[32] = {0};
    char str1[32] = {0};

    scanf("%31[^\n]s", str);

    int len = strlen(str);
    int j;
    for (int i = 0; i < len; i++)
    {
        int has = 0;
        for (j = 0; j < strlen(str1); j++)
        {
            if (str1[j] == str[i])
            {
                has = 1;
                break;
            }
        }
        if (has == 0)
        {
            str1[j] = str[i];
        }
    }
    printf("%s \n", str1);
    return 0;
}
