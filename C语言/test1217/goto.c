#include <stdio.h>

void clear_stdin()
{
    // 死循环让getchar不断读取程序中输入的字符直到 \n 为止,目的清理所有垃圾数据
    while (getchar() != '\n');
}
int main()
{
    int day;
    printf("请输入日期：\n");
    int n;
loop:
    n = scanf("%d",&day);
    clear_stdin();
    if(n != 1 || !(day >= 1 && day <= 31))
    {
        printf("输入错误，请重新输入：\n");

        goto loop;
    }
    printf("您输入的日期是%d\n",day);
    
    return 0;
}