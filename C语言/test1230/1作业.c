// 日期：12月30
// 作业内容：设计一个简单的文字战争策略游戏                                                              作业内容：设计一个简单的文字战争策略游戏
// 游戏账号  密码  ID  城池等级   vip贵族等级  资源（木材数量  砖石数量 铁块数量）
// 功能：
// 1、注册账号
// 2、登录
// 3、每个玩家初始资源  城池等级=1  vip=1  木材  砖石  铁矿  各 1万
// 4、刷雷峰塔副本，获取资源（每提升一层，获取的资源越多  使用算法实现）
// 5、城池升级 ，每次提升一级 消耗的资源 成倍数提升

#include <stdio.h>
#include <math.h>

long city(int city, int wood)
{
    if (city >= 0 && wood >= 0)
    {
        int sum = wood;
        if (city == 1)
        {
            return wood;
        }
        if (wood / (city * city) >= 0)
        {
            return wood / (city * city);
        }
        else
        {
            return sum;
        }
    }
}

long copy(int level)
{
    if (level >= 0)
    {
        if (level < 1)
            return 5000;

        if (level == 1)
            return 10000;
        else
        {
            return level * 10000 + copy(level - 1);
        }
    }
}

int main(int argc, char const *argv[])
{
    int n, m;
    int city_level = 1;
    int level;
    int vip = 1;
    unsigned int wood = 10000, diamond = 10000, iron = 10000;

    printf("初始木材=%d  初始钻石=%d  初始铁矿=%d\n", wood, diamond, iron);

    printf("欢迎挑战雷峰塔副本\n");
    printf("请输入您的攻击力：\n");
    scanf("%d", &n);

    level = n * 0.1;
    printf("欢迎挑战第%d关：\n", level);
    if (level >= 1)
    {
        printf("挑战成功！\n");
    }
    else
    {
        printf("挑战失败！资源减半！\n");
    }
    wood = copy(level);
    diamond = copy(level);
    iron = copy(level);
    printf("当前木材=%d  当前钻石=%d  当前铁矿=%d\n", wood, diamond, iron);

    printf("您要提升城池到几级：\n");
    scanf("%d", &m);
    wood = city(m, wood);
    diamond = city(m, diamond);
    iron = city(m, iron);
    if (wood >= 1)
    {
        printf("提升成功！\n");
        printf("当前木材=%d  当前钻石=%d  当前铁矿=%d\n", wood, diamond, iron);
    }
    else
    {
        printf("提升失败！资源不够！\n");
    }

    return 0;
}
