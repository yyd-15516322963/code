// 输入三个整数 x、y、z，请把这三个数由小到大输出。
#include<stdio.h>
int main()
{
    int a,b,c,t;
    scanf("%d %d %d",&a,&b,&c);

    // min = a<b?(a<c?a:c):(b<c?b:c);
    // max = a>b?(a>c?a:c):(b>c?b:c);
    if(a>b)
    {
        t=a;a=b;b=t;
    }
    if(a>c)
    {
        t=a;a=c;c=t;
    }
    if(b>c)
    {
        t=c;c=b;b=t;
    }
    printf("%d %d %d\n",a,b,c);
}