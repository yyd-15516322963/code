// 1、编写一个程序，该程序读取输入直到遇到#字符，然后报告读取的空格数目、读取的换行符数目以及读取的所有其他字符数目。
// 例如：输入:a 3 bc%#
// 输出:spaces = 2, lines = 1, characters= 5
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    char ch;
    int spaces = 0, lines = 1, characters = 0;
    while ((ch = getchar()) != '#')
    {
        if (ch == ' ')
        {
            spaces++;
        }
        else if (ch == '\n')
        {
            lines++;
        }
        else
        {

            characters++;
        }
    }
    printf("lines = %d\n", lines);
    printf("spaces = %d\n", spaces);
    printf("characters = %d\n", characters);
    return 0;
}
