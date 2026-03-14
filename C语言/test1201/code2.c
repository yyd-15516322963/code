// 如果是个奇数，则下一步变成 3N+1
// 如果是个偶数，则下一步变成 N/2
// 对于任意非零自然数N，经上述变换最终都将落入4-2-1序列的宿命。这就是著名的角谷猜想，或称冰雹猜想。
#include<stdio.h>
int main()
{
    int num1,num2;
    printf("请输入两个数字：\n");
    scanf("%d %d",&num1,&num2);
    while (num1 != 1)
    {
        if(num1 % 2 == 0)
        {
            num1 = num1 / 2;
        }else
            num1 = num1 * 3 + 1;

    printf("%d ",num1);
    }
    printf("\n");


    while (num2 != 1)
    {
        if(num2 % 2 == 0)
        {
            num2 = num2 / 2;
        }else
            num2 = num2 * 3 + 1;

    printf("%d ",num2);
    }

}