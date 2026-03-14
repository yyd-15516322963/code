// 题目 6：1 到 100 的累加和分别用 for、while、do-while 循环计算 1+2+...+100 的和并输出。
#include<stdio.h>
int main()
{
    int sum = 0;
    // for
    for(int i = 1;i <=100;i++)
    {
        sum += i;
    }
    printf("1:%d\n",sum);

    //while
    // int n =1;
    // while (n <=100)
    // {
    //     sum += n;
    //     n++;
    // }
    // printf("2:%d\n",sum);

    //do-while
    // do{
    //     sum += n;
    //     n++;
    // }while(n<=100);
    // printf("3:%d\n",sum);
    

}