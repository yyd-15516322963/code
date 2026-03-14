#include <stdio.h>

int main()
{
    int level;
    printf("请输入您的会员等级：\n");
    scanf("%d",&level);

    switch (level)
    {
    case 0:
    case 1:
    case 2:
        printf("level 1\n");
        break;
    
    case 3 ...10:
        printf("level 2\n");
        break;
    default:
        printf("level\n");
        break;
    }

    return 0;
}