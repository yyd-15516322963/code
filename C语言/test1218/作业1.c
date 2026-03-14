// 定义10个元素整型数组，数组元素数据通过键盘输入，请找出数组当中的最大、小值，并输出它们的和。
// 输出
// 输入十个数字：4 5 6 7 8 9 1 2 3 0
// min:0,max:9
// min+max=9
// 输入十个数字：-34 6 98 33 -25 1 25 45 78 63
// min:-34,max:98
// min+max=64

#include <stdio.h>

void clear_stdin()
{
    // 死循环让getchar不断读取程序中输入的字符直到 \n 为止,目的清理所有垃圾数据
    while (getchar() != '\n')
        ;
}
int main(int argc, char const *argv[])
{

    int arr[10] = {0};
    int sum = 0;
    for (int i = 0; i < 10; i++)
    {
        printf("请输入第%d个数\n", i + 1);

        int ret = 0;
        do
        {
            ret = scanf("%d", &(arr[i]));

            if (ret == 0)
            {
                printf("输入错误，请重新输入\n");
            }
            clear_stdin();
        } while (ret != 1);
    }
    int max = arr[0], min = arr[0];
    for (int i = 1; i < 10; i++)
    {
        if (arr[i] > max)
        {
            max = arr[i];
        }
        if (arr[i] < min)
        {
            min = arr[i];
        }
    }
    sum = max + min;
    printf("max = %d\n", max);
    printf("min = %d\n", min);
    printf("sum = %d\n", sum);
    return 0;
}