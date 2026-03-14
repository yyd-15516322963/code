#include <stdio.h>
#include <stdlib.h>
/*
    1.最笨的办法写单链表的存储逻辑
         56   97  714  89   532
    2.引入理论概念
         (1)数据域和指针域
            单链表由数据域和指针域两部分组成
         (2)节点/结点
            每一次malloc申请堆空间得到的那个指针就是一个节点/结点
            头结点
                第一个申请得到的节点,叫做头结点,其他的节点叫做普通节点
*/
// 定义结构体表示单链表
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
    headnode->next = NULL;
    return headnode;
}

// 封装函数:实现尾插新的节点
void list_insert_tail(int newdata, struct siglelist *head)
{
    // 准备新的节点
    struct siglelist *newnode = malloc(sizeof(struct siglelist));
    newnode->data = newdata;
    newnode->next = NULL;

    // 把新的节点插入到链表的尾部
    // head->next=newnode; //不正确

    // 正确的写法:定义p找到链表的尾部,然后把newnode存放到p的后面
    // 不要偷懒:一定画个图分析
    struct siglelist *p = head;

    while (p->next != NULL)
    {
        p = p->next;
    }
    p->next = newnode;
}

void list_insert_behind(int newdata, int olddata, struct siglelist *head)
{
    struct siglelist *newnode = malloc(sizeof(struct siglelist));
    newnode->data = newdata;
    newnode->next = NULL;

    struct siglelist *p = head;
    while (p != NULL)
    {
        if (p->data == olddata)
        {
            newnode->next = p->next;
            p->next = newnode;
        }
        p = p->next;
    }
    printf("要插入的地方不存在\n");
}

struct siglelist *list_insert_before(int newdata, int olddata, struct siglelist *head)
{

    int flag = 0;
    struct siglelist *newnode = malloc(sizeof(struct siglelist));
    newnode->data = newdata;
    newnode->next = NULL;

    struct siglelist *p = head;
    if (p->data == olddata)
    {
        newnode->next = p;
        head = newnode;
        return head;
    }

    while (p->next != NULL)
    {
        if (p->next->data == olddata)
        {
            flag = 1;
            break;
        }
        p = p->next;
    }

    if (flag == 1)
    {
        newnode->next = p->next;
        p->next = newnode;
    }
    else
        printf("要插入的地方不存在\n");
}

// 删除
struct siglelist *delete(int olddata, struct siglelist *head)
{
    struct siglelist *p = head;
    struct siglelist *q = p->next;

    if (p->data == olddata)
    {
        p->next = NULL;
        free(p);
        head = q;
        return head;
    }

    while (p->next != NULL)
    {
        if (q->data == olddata)
        {
            p->next = q->next;
            q->next = NULL;
            free(q);
            break;
        }
        p = p->next;
        q = q->next;
    }
}

// 修改
void list_change(int olddata, int newdata, struct siglelist *head)
{
    struct siglelist *p = head;

    while (p != NULL)
    {
        if (p->data == olddata)
        {
            p->data = newdata;
        }
        p = p->next;
    }
}

void list_show(struct siglelist *head)
{

    // 使用一个临时指针 p 来遍历，避免修改头指针 head
    struct siglelist *p = head;
    // 当 p 不为 NULL 时，说明还有节点可以访问
    while (p != NULL)
    {
        printf("%d\n", p->data);
        p = p->next; // 移动到下一个节点
    }
}

void list_show_digui(struct siglelist *head)
{
    if(head == NULL)
    {
        return;
    }
    list_show_digui(head->next);
    printf("%d\n",head->data);
}

// 链表的销毁释放
void list_release(struct siglelist *head)
{
    struct siglelist *p = head;

    while (p != NULL)
    {
        struct siglelist *q = p->next;
        free(p);
        p = q;
    }
}

int main()
{
    // 初始化链表的头结点
    struct siglelist *myhead = list_init(56);

    // 其他的节点往头结点代表的链表后面插入
    list_insert_tail(97, myhead);
    list_insert_tail(714, myhead);
    list_insert_tail(89, myhead);
    list_insert_tail(532, myhead);

    list_show(myhead);

    myhead = list_insert_before(333, 56, myhead);
    // list_insert_behind(52, 89, myhead);
    // list_insert_behind(52, 55, myhead);
    printf("================\n");

    // delete(97, myhead);

    // list_change(56, 65, myhead);
    // myhead = delete(56, myhead);

    list_show(myhead);
    printf("================\n");
    list_show_digui(myhead);
    // list_show(5, myhead);

    // list_release(myhead);
    return 0;
}