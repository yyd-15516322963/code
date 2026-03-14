#include <stdio.h>

int main(int argc, char const *argv[])
{

    // int a = getchar();
    // printf("%d\n",a);
    // printf("%c\n",a);
    // return 0;
    printf("请输出年龄和身高：\n");

    int age, height;
    int n = scanf("%d%d", &age, &height);


    // 清理上一次的换行符,避免被下面的 %c 接收
    int ret = getchar();
    while(n != 2)
    {
        printf("请输出年龄和身高：\n");
        char x[10];
        fgets(x, 10,stdin);
        n = scanf("%d%d", &age, &height);

    }
    printf("请给自己打个分 A~D! \n");

    char score;
    scanf("%c", &score);

    printf("你给自己打的分为 '%c' ascii '%d' \n", score, score);

    return 0;
}