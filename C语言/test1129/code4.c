// 题目 14：模运算的实际应用
// 输入一个整数，判断其是否为偶数（用%2），再判断其是否能被 3 和 5 同时整除。
#include<stdio.h>
int main()
{
    int n;
    printf("请输入一个整数：");
    scanf("%d",&n);
    if( n % 2 ==0)
    {
        printf("偶数");

    }
    else if(n % 3 == 0 && n % 5 == 0)
    {
        printf("能被 3 和 5 同时整除");
    }else{
        printf("都不是");
    }
}