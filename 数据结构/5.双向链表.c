#include <stdio.h>
#include <stdlib.h>

struct doublelist
{
    // 数据域:存放真实数据的那些变量,就是数据域
    int data; // 存放真实数据
    // 指针域
    struct doublelist *next; // 存放下一个数据在内存中的首地址-->方便寻找下一个
    struct doublelist *prev;
};

// 封装函数:初始化单链表的头结点
struct doublelist *list_init(int newdata)
{
    struct doublelist *headnode = malloc(sizeof(struct doublelist));
    headnode->data = newdata;
    headnode->next = NULL;
    headnode->prev = NULL;
    return headnode;
}

// 封装函数:实现尾插新的节点
void list_insert_tail(int newdata, struct doublelist *head)
{
    // 准备新的节点
    struct doublelist *newnode = malloc(sizeof(struct doublelist));
    newnode->data = newdata;
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

void list_insert_behind(int newdata, int olddata, struct doublelist *head)
{
    struct doublelist *newnode = malloc(sizeof(struct doublelist));
    newnode->data = newdata;
    newnode->next = NULL;
    newnode->prev = NULL;

    struct doublelist *p = head;
    // struct doublelist *tail = head;

    // while (tail->next != NULL)
    // {
    //     tail = tail->next;
    // }
    // if (tail->data == olddata)
    // {
    //     tail->next = newnode;
    //     newnode->prev = tail;
    // }

    while (p != NULL)
    {
        if (p->data == olddata)
        {
            newnode->next = p->next;
            newnode->prev = p;

            p->next = newnode;
            if (p->next != NULL)
                p->next->prev = newnode;
        }
        p = p->next;
    }
}

// 删除
struct doublelist *delete(int olddata, struct doublelist *head)
{
    struct doublelist *p = head;
    struct doublelist *q = p->next;

    if (p->data == olddata)
    {
        p->next->prev = NULL;
        p->next = NULL;
        free(p);
        head = q;
        return head;
    }

    while (p != NULL)
    {
        if (p->data == olddata)
        {
            p->prev->next = p->next;

            if (p->next != NULL)
            {
                p->next->prev = p->prev;
            }
            q = p->prev;
            p->prev = NULL;
            p->next = NULL;
            free(p);
            p = q;
        }
        p = p->next;
    }
}

void list_release(struct doublelist *head)
{
    struct doublelist *p = head->next;

    while (p != NULL)
    {
        struct doublelist *q = p->next;
        p->prev->next = p->next;

        if (p->next != NULL)
        {
            p->next->prev = p->prev;
        }
        p->prev = NULL;
        p->next = NULL;
        free(p);
        p = q;
    }
    head->next = NULL;
    free(head);
    head = NULL;
}

void list_show(struct doublelist *head)
{

    struct doublelist *p = head;
    // 当 p 不为 NULL 时，说明还有节点可以访问
    while (p->next != NULL)
    {
        printf("%d\n", p->data);
        p = p->next; // 移动到下一个节点
    }
    printf("%d\n", p->data);
}

int main(int argc, char const *argv[])
{

    struct doublelist *myhead = list_init(10);

    list_insert_tail(11, myhead);
    list_insert_tail(12, myhead);
    list_insert_tail(13, myhead);
    list_insert_tail(14, myhead);
    list_insert_tail(11, myhead);

    // list_insert_behind(88, 10, myhead);

    // myhead = delete(10, myhead);
    delete(11, myhead);

    list_show(myhead);

    list_release(myhead);
    return 0;
}
