#include <stdio.h>
#include <stdlib.h>

struct siglelist
{
    // 数据域:存放真实数据的那些变量,就是数据域
    int data; // 存放真实数据
    // 指针域
    struct siglelist *next; // 存放下一个数据在内存中的首地址-->方便寻找下一个
};

// 封装函数:初始化单链表的头结点
struct siglelist *list_init(int newdata)
{
    struct siglelist *headnode = malloc(sizeof(struct siglelist));
    headnode->data = newdata;
    headnode->next = headnode;
    return headnode;
}

// 封装函数:实现尾插新的节点
void list_insert_tail(int newdata, struct siglelist *head)
{
    // 准备新的节点
    struct siglelist *newnode = malloc(sizeof(struct siglelist));
    newnode->data = newdata;
    newnode->next = NULL;

    struct siglelist *p = head;

    while (p->next != head)
    {
        p = p->next;
    }

    newnode->next = head;
    p->next = newnode;
}

struct siglelist *list_insert_before(int newdata, int olddata, struct siglelist *head)
{
    // 准备新的节点
    struct siglelist *newnode = malloc(sizeof(struct siglelist));
    newnode->data = newdata;
    newnode->next = NULL;
    // 定义标志位,标记是否找到olddata
    int flag = 0;
    // 找到olddata
    struct siglelist *p = head;

    if (p->data == olddata)
    {
        newnode->next = head;
        
        while (p->next != head)
        {
            p = p->next;
        }
        p->next = newnode;
        head = newnode;
        return head;
    }
    do
    {
        if (p->next->data == olddata) // 找到olddata了
        {
            flag = 1;
            break;
        }
        p = p->next;
    } while (p->next != head);
    // 判断olddata不存在的情况
    if (flag == 1)
    { 
        // 把newnode插入到olddata代表的节点的后面
        newnode->next = p->next;
        p->next = newnode;
    }
    else
        printf("对不起,%d根本不存在!\n", olddata);
}

void list_insert_mid(int newdata, int olddata, struct siglelist *head)
{
    // 定义标志位,标记是否找到olddata
    int flag = 0;
    // 找到olddata
    struct siglelist *p = head;
    do
    {
        if (p->data == olddata) // 找到olddata了
        {
            flag = 1;
            break;
        }
        p = p->next;
    } while (p != head);
    // 判断olddata不存在的情况
    if (flag == 0)
    {
        printf("对不起,%d根本不存在!\n", olddata);
        return;
    }

    // 准备新的节点
    struct siglelist *newnode = malloc(sizeof(struct siglelist));
    newnode->data = newdata;
    newnode->next = NULL;

    // 把newnode插入到olddata代表的节点的后面
    newnode->next = p->next;
    p->next = newnode;
}

void list_show(struct siglelist *head)
{

    // 使用一个临时指针 p 来遍历，避免修改头指针 head
    struct siglelist *p = head;
    // 当 p 不为 NULL 时，说明还有节点可以访问
    while (p->next != head)
    {
        printf("%d\n", p->data);
        p = p->next; // 移动到下一个节点
    }
    printf("%d\n", p->data);
}

int main(int argc, char const *argv[])
{
    struct siglelist *myhead = list_init(23);

    list_insert_tail(74, myhead);
    list_insert_tail(82, myhead);
    list_insert_tail(97, myhead);
    list_insert_tail(36, myhead);

    myhead = list_insert_before(11, 23, myhead);
    list_insert_before(22, 82, myhead);
    list_insert_mid(12, 36, myhead);

    list_show(myhead);

    return 0;
}
