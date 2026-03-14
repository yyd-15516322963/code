// 题目：打印楼梯，同时在楼梯上方打印两个笑脸。

#include <stdio.h>

int main(int argc, char const *argv[])
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < i; j++)
        {
            printf("__");
        }
        printf("\n");
    }

    return 0;
}
