#include <stdio.h>
void clear_stdin()
{
    // 死循环让getchar不断读取程序中输入的字符直到 \n 为止,目的清理所有垃圾数据
    while (getchar() != '\n');
}
int main(int argc, char const *argv[])
{

    int ret,M;
    do{
        printf("请输入月份：\n");
        ret = scanf("%d",&M);
        clear_stdin();

    }while((ret != 1) || !(M >= 1 && M <= 12));
    printf("您输入的月份为%d\n",M);
    return 0;
}
