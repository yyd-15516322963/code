#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct doublelist
{
    // 数据域:存放真实数据的那些变量,就是数据域
    char data[20]; // 存放真实数据
    // 指针域
    struct doublelist *next; // 存放下一个数据在内存中的首地址-->方便寻找下一个
    struct doublelist *prev;
};

// 封装函数:初始化单链表的头结点
struct doublelist *list_init(char *newdata)
{
    struct doublelist *headnode = malloc(sizeof(struct doublelist));
    strcpy(headnode->data, newdata);
    headnode->next = NULL;
    headnode->prev = NULL;
    return headnode;
}

// 封装函数:实现尾插新的节点
void list_insert_tail(char *newdata, struct doublelist *head)
{
    // 准备新的节点
    struct doublelist *newnode = malloc(sizeof(struct doublelist));
    strcpy(newnode->data, newdata);
    newnode->next = NULL;
    newnode->prev = NULL;

    struct doublelist *prev = head;
    struct doublelist *p = head;

    while (p->next != NULL)
    {
        p = p->next;
    }

    newnode->prev = p;
    p->next = newnode;
}



void list_show_next(struct doublelist *head)
{

    struct doublelist *p = head;
    // 当 p 不为 NULL 时，说明还有节点可以访问
    while (p->next != NULL)
    {
        printf("%s\n", p->data);
        p = p->next; // 移动到下一个节点
    }
    printf("%s\n", p->data);
}

void list_show_prev(struct doublelist *head)
{

    struct doublelist *p = head;
    // 移动到最后一个节点
    while (p->next != NULL)
    {
        p = p->next;
    }
    // 向前访问直到头节点
    while (p != NULL)
    {
        printf("%s\n", p->data);
        p = p->prev; // 移动到下一个节点
    }
    printf("%s\n", head->data);
}

int main(int argc, char const *argv[])
{
    struct doublelist *myhead = list_init("晴天");

    list_insert_tail("夜曲", myhead);
    list_insert_tail("海阔天空", myhead);
    list_insert_tail("龙卷风", myhead);
    list_insert_tail("真的爱你", myhead);

    char n;
    printf("选择您要遍历的顺序（n顺序 p逆序）\n");
    scanf("%c", &n);
    switch (n)
    {
    case 'n':
        printf("=======下面为顺序遍历结果=======\n");
        list_show_next(myhead);
        break;
    case 'p':
        printf("=======下面为逆序遍历结果=======\n");
        list_show_prev(myhead);
        break;
    default:
        break;
    }
    system("madplay /Users/33513/Desktop/Beyond-真的爱你.mp3 &");
    return 0;
}
