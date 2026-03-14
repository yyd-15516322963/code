// C 语言编程面试题：猴子吃桃子问题
// 题目描述
// 有一只猴子吃一堆桃子，它每天都会吃掉当前桃子数量的一半多一个。
// 当吃到第 n 天（n≥1）时，发现只剩下 1 个桃子。
// 请编写 C 语言程序，计算这堆桃子最初有多少个。
// 要求
// 输入：从控制台输入一个正整数 n（表示第 n 天只剩 1 个桃子）。
// 输出：输出桃子最初的数量。

#include <stdio.h>

void clear_stdin()
{

    // 死循环让getchar不断读取程序中输入的字符直到 \n 为止,目的清理所有垃圾数据
    while (getchar() != '\n')
        ;
}

long int monkey_peach(int i)
{
    if (i > 1 || i == 1)
    {
        if (i == 1)
        {
            return 1;
        }
        else
            return (monkey_peach(i - 1) + 1) * 2;
    }
    else
        return -1;
}

int main(int argc, char const *argv[])
{
    int n, num;
    do
    {
        printf("请输入剩一个桃子的天数：");
        clear_stdin();
        num = scanf("%d", &n);
    } while (num != 1);

    printf("%ld \n", monkey_peach(n));

    return 0;
}
