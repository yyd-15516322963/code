#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>

// 用户信息
struct user_info
{
    int age;
    char gender[10];
    char username[20];
    char userpw[20];
    char ID[20];
    struct user_info *prev;
    struct user_info *next;
};

// 航班信息
struct plane_info
{
    char shift[10];
    char airlinename[20];
    char startlocation[20];
    char endlocation[20];
    char starttime[20];
    char endtime[20];
    struct plane_info *prev;
    struct plane_info *next;
};

// 用户头节点初始化
struct user_info *user_init(char *new_username, int new_age, char *new_gender, char *new_userpw, char *new_ID)
{
    struct user_info *headnode = malloc(sizeof(struct user_info));
    headnode->age = new_age;
    strcpy(headnode->gender, new_gender);
    strcpy(headnode->username, new_username);
    strcpy(headnode->userpw, new_userpw);
    strcpy(headnode->ID, new_ID);
    headnode->prev = NULL;
    headnode->next = NULL;
    return headnode;
}

// 航班头节点初始化
struct plane_info *plane_init()
{
    struct plane_info *headnode = malloc(sizeof(struct plane_info));
    headnode->prev = NULL;
    headnode->next = NULL;
    return headnode;
}

// 用户创建新节点
struct user_info *user_creat(char *new_username, int new_age, char *new_gender, char *new_userpw, char *new_ID)
{
    struct user_info *newnode = malloc(sizeof(struct user_info));
    newnode->age = new_age;
    strcpy(newnode->gender, new_gender);
    strcpy(newnode->username, new_username);
    strcpy(newnode->userpw, new_userpw);
    strcpy(newnode->ID, new_ID);
    newnode->prev = NULL;
    newnode->next = NULL;
    return newnode;
}

// 航班创建新节点
struct plane_info *plane_creat(char *new_shift, char *new_airlinename, char *new_startlocation, char *new_endlocation, char *new_starttime, char *new_endtime)
{
    struct plane_info *newnode = malloc(sizeof(struct plane_info));
    strcpy(newnode->shift, new_shift);
    strcpy(newnode->startlocation, new_startlocation);
    strcpy(newnode->endlocation, new_endlocation);
    strcpy(newnode->airlinename, new_airlinename);
    strcpy(newnode->starttime, new_starttime);
    strcpy(newnode->endtime, new_endtime);
    newnode->prev = NULL;
    newnode->next = NULL;
    return newnode;
}

// 尾插用户信息
void user_info_tail(char *new_username, int new_age, char *new_gender, char *new_userpw, char *new_ID, struct user_info *userhead)
{
    struct user_info *newnode = user_creat(new_username, new_age, new_gender, new_userpw, new_ID);
    struct user_info *p = userhead;
    while (p->next != NULL)
    {
        p = p->next;
    }
    p->next = newnode;
    newnode->prev = p;
}

// 尾插航班信息
void plane_info_tail(char *new_shift, char *new_airlinename, char *new_startlocation, char *new_endlocation, char *new_starttime, char *new_endtime, struct plane_info *planehead)
{
    struct plane_info *newnode = plane_creat(new_shift, new_airlinename, new_startlocation, new_endlocation, new_starttime, new_endtime);
    struct plane_info *p = planehead;
    while (p->next != NULL)
    {
        p = p->next;
    }
    p->next = newnode;
    newnode->prev = p;
}

// 根据用户名和密码删除用户信息
struct user_info *user_info_del(char *old_username, char *old_userpw, struct user_info *userhead)
{
    struct user_info *p = userhead->next;
    if (userhead->next == NULL)
    {
        printf("没有元素可以删除\n");
        return userhead;
    }
    while (p != NULL)
    {
        if (strcmp(p->username, old_username) == 0 && strcmp(p->userpw, old_userpw) == 0)
        {
            p->prev->next = p->next;
            if (p->next != NULL)
            {
                p->next->prev = p->prev;
            }
            p->prev = NULL;
            p->next = NULL;
            free(p);
            p = NULL;
            printf("用户【%s】注销成功！\n", old_username);
            return userhead;
        }
        p = p->next;
    }
    printf("未找到用户【%s】，注销失败！\n", old_username);
    return userhead;
}

// 根据班次和起飞时间删除航班信息
struct plane_info *plane_info_del(char *old_shift, char *old_starttime, struct plane_info *planehead)
{
    struct plane_info *p = planehead->next;
    if (planehead->next == NULL)
    {
        printf("没有元素可以删除\n");
        return planehead;
    }
    while (p != NULL)
    {
        if (strcmp(p->shift, old_shift) == 0 && strcmp(p->starttime, old_starttime) == 0)
        {
            p->prev->next = p->next;
            if (p->next != NULL)
            {
                p->next->prev = p->prev;
            }
            p->prev = NULL;
            p->next = NULL;
            free(p);
            p = NULL;
            printf("航班【%s】删除成功！\n", old_shift);
            return planehead;
        }
        p = p->next;
    }
    printf("未找到航班【%s】，删除失败！\n", old_shift);
    return planehead;
}

// 查找用户信息
int user_info_select(char *old_username, char *old_userpw, struct user_info *userhead)
{
    struct user_info *p = userhead->next;
    if ((strcmp(userhead->username, old_username) == 0 && strcmp(userhead->userpw, old_userpw) == 0))
    {
        printf("进入管理员系统！\n");
        return 1;
    }
    else
    {

        while (p != NULL)
        {
            if (strcmp(p->username, old_username) == 0 && strcmp(p->userpw, old_userpw) == 0)
            {
                printf("================================ 查找用户结果 ==========================\n");
                printf("用户名：%s\t 年龄：%d\t 性别：%s\t 用户ID：%s \n", p->username, p->age, p->gender, p->ID);
                return 0;
            }
            p = p->next;
        }
        printf("抱歉！找不到该用户信息！\n");
        return 0;
    }
}

// 查找航班信息
void plane_info_select(char *old_shift, struct plane_info *planehead)
{
    struct plane_info *p = planehead->next;
    while (p != NULL)
    {
        if (strcmp(p->shift, old_shift) == 0)
        {
            printf("============================================ 查找航班结果 ============================================\n");
            printf("班次：%-15s航空公司：%-20s起飞地点：%-15s到达地点：%-15s起飞时间：%-15s到达时间：%s\n", p->shift, p->airlinename, p->startlocation, p->endlocation, p->starttime, p->endtime);
            return;
        }
        p = p->next;
    }
    printf("抱歉！找不到该航班信息！\n");
}

// 修改用户信息
void user_info_change()
{
}

// 修改航班信息
void plane_info_change()
{
}

// 展示用户信息
void user_info_show(struct user_info *userhead)
{
    struct user_info *p = userhead;
    while (p->next != NULL)
    {
        printf("用户名：%s\t 年龄：%d\t 性别：%s\t 用户ID：%s \n", p->username, p->age, p->gender, p->ID);
        p = p->next;
    }
    printf("用户名：%s\t 年龄：%d\t 性别：%s\t 用户ID：%s \n", p->username, p->age, p->gender, p->ID);
}// 展示航班信息
void plane_info_show(struct plane_info *planehead)

{
    struct plane_info *p = planehead->next;
    while (p->next != NULL)
    {
        printf("班次：%-15s航空公司：%-20s起飞地点：%-15s到达地点：%-15s起飞时间：%-15s到达时间：%s\n", p->shift, p->airlinename, p->startlocation, p->endlocation, p->starttime, p->endtime);
        p = p->next;
    }
    printf("班次：%-15s航空公司：%-20s起飞地点：%-15s到达地点：%-15s起飞时间：%-15s到达时间：%s\n", p->shift, p->airlinename, p->startlocation, p->endlocation, p->starttime, p->endtime);
}

int main(int argc, char const *argv[])
{
    int n;
    char user_name[10];
    char user_password[10];
    int user_age;
    char user_gender[10];
    char user_ID[20];
    char plane_shift[10];
    char plane_starttime[10];
    char plane_airlinename[20];
    char plane_startlocation[20];
    char plane_endlocation[20];
    char plane_endtime[20];

    struct user_info *usermyhead = user_init("管理员", 21, "男", "123456", "000001");
    printf("\n");
    user_info_tail("张三", 21, "男", "111111", "943623", usermyhead);
    user_info_tail("李四", 29, "男", "222222", "972541", usermyhead);
    user_info_tail("王五", 47, "女", "333333", "973832", usermyhead);
    user_info_tail("赵六", 33, "女", "444444", "912722", usermyhead);
    user_info_tail("王三", 60, "男", "555555", "984643", usermyhead);
    user_info_tail("赵四", 33, "男", "666666", "907623", usermyhead);

    printf("=========================== 用户信息表 =========================\n");
    user_info_show(usermyhead);

    struct plane_info *planemyhead = plane_init();
    plane_info_tail("CZ1167", "南方航空", "广州", "武汉", "10:09", "11:23", planemyhead);
    plane_info_tail("HU1763", "海南航空", "重庆", "长沙", "13:13", "15:46", planemyhead);
    plane_info_tail("SD3173", "山东航空", "济南", "郑州", "18:55", "19:35", planemyhead);
    plane_info_tail("SD1567", "山东航空", "济南", "南京", "11:23", "12:51", planemyhead);
    plane_info_tail("CZ3387", "南方航空", "南宁", "北京", "9:55", "14:00", planemyhead);
    printf("============================================================= 航班信息表 =============================================================\n");
    plane_info_show(planemyhead);

    while (1)
    {
        printf("=======================================================================\n");
        printf("请选择您要进行的操作---------(1)查询账户(2)注销账户(3)查询航班(0)退出\n");
        printf("=======================================================================\n");
        scanf(" %d", &n);
        getchar();
        if (n == 0)
        {
            printf("欢迎下次使用！\n");
            break;
        }
        switch (n)

        {
        case 1:
            printf("请输入你要登录的用户！\n");
            scanf("%s", user_name);
            printf("请输入密码！\n");
            scanf("%s", user_password);
            int m = user_info_select(user_name, user_password, usermyhead);
            if (m == 1)
            {
                while (1)
                {
                    char manager;
                    printf("=============================================================================\n");
                    printf("欢迎使用管理员系统---------(a)添加用户信息(b)添加航班信息(c)删除航班信息(q)退出\n");
                    printf("=============================================================================\n");
                    scanf(" %c", &manager);
                    getchar();
                    if (manager == 'q')
                    {
                        printf("欢迎下次使用管理员系统！\n");
                        break;
                    }
                    switch (manager)
                    {
                    case 'a':
                        printf("请输入你要注册的用户！\n");
                        scanf("%s", user_name);
                        printf("请输入密码！\n");
                        scanf("%6s", user_password);
                        printf("请输入年龄！\n");
                        scanf("%d", &user_age);
                        while (1)
                        {
                            if (user_age > 0)
                            {
                                break;
                            }
                            else
                            {
                                printf("请重新输入年龄！\n");
                                scanf("%d", &user_age);
                            }
                        }

                        printf("请输入性别！\n");
                        scanf("%s", user_gender);
                        printf("请输入ID！\n");
                        scanf("%6s", user_ID);
                        user_info_tail(user_name, user_age, user_gender, user_password, user_ID, usermyhead);
                        printf("用户添加成功！\n");
                        user_info_show(usermyhead);
                        break;
                    case 'b':
                        printf("请输入你要添加航班的班次！\n");
                        scanf("%s", plane_shift);
                        printf("请输入你要添加航班的航空公司！\n");
                        scanf("%s", plane_airlinename);
                        printf("请输入你要添加航班的起飞地点！\n");
                        scanf("%s", plane_startlocation);
                        printf("请输入你要添加航班的到达地点！\n");
                        scanf("%s", plane_endlocation);
                        printf("请输入你要添加航班的起飞时间！\n");
                        scanf("%s", plane_starttime);
                        printf("请输入你要添加航班的到达时间！\n");
                        scanf("%s", plane_endtime);

                        plane_info_tail(plane_shift, plane_airlinename, plane_startlocation, plane_endlocation, plane_starttime, plane_endtime, planemyhead);
                        printf("航班添加成功！\n");
                        plane_info_show(planemyhead);
                        break;
                    case 'c':
                        printf("请输入你要删除的航班信息！\n");
                        scanf("%s", plane_shift);
                        printf("请输入你要删除的航班起飞时间！\n");
                        scanf("%s", plane_starttime);
                        planemyhead = plane_info_del(plane_shift, plane_starttime, planemyhead);
                        plane_info_show(planemyhead);
                        break;
                    default:
                        printf("输入有误，为防止恶意破坏，退出管理员系统！\n");
                        break;
                    }
                }
            }
            break;

        case 2:
            printf("请输入你要注销的用户！\n");
            scanf("%s", user_name);
            printf("请输入密码！\n");
            scanf("%s", user_password);
            printf("请再次输入密码！\n");
            scanf("%s", user_password);
            usermyhead = user_info_del(user_name, user_password, usermyhead);
            user_info_show(usermyhead);
            break;
        case 3:
            printf("请输入你要查询航班的班次！\n");
            scanf("%s", plane_shift);
            plane_info_select(plane_shift, planemyhead);
            break;
        default:
            printf("输入有误，程序退出，如果使用请再次进入！\n");
            break;
        }
        return 0;
    }
}