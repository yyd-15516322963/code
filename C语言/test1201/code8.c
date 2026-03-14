// 古典问题（兔子生崽）：有一对兔子，从出生后第3个月起每个月都生一对兔子，
// 小兔子长到第三个月后每个月又生一对兔子，假如兔子都不死，
// 问每个月的兔子总数为多少？（输出前40个月即可）
// 程序分析：兔子的规律为数列1,1,2,3,5,8,13,21....，
// 即下个月是上两个月之和（从第三个月开始）。
#include<stdio.h>
int main()
{
    int list[40];
    int n;
    list[0] = 1;
    list[1] = 1;
    printf("%d\n",list[0]);
    printf("%d\n",list[1]);
    for(int i = 2;i<40;i++)
    {
        list[i] = list[i-1] + list[i-2];
        printf("%d\t",list[i]);
        printf("\n");
    }
}