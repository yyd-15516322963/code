#include <stdio.h>
#include <stdlib.h>

// struct siglelist
// {
//     // 数据域:存放真实数据的那些变量,就是数据域
//     int data; // 存放真实数据
//     // 指针域
//     struct siglelist *next; // 存放下一个数据在内存中的首地址-->方便寻找下一个
// };

// // 封装函数:初始化单链表的头结点
// struct siglelist *list_init(int newdata)
// {
//     struct siglelist *headnode = malloc(sizeof(struct siglelist));
//     headnode->data = newdata;
//     headnode->next = NULL;
//     return headnode;
// }

// void list_insert_tail(int newdata, struct siglelist *head)
// {
//     // 准备新的节点
//     struct siglelist *newnode = malloc(sizeof(struct siglelist));
//     newnode->data = newdata;
//     newnode->next = NULL;

//     struct siglelist *p = head;

//     while (p->next != NULL)
//     {
//         p = p->next;
//     }
//     p->next = newnode;
// }

// //
// struct siglelist *delete(struct siglelist *head)
// {
//     struct siglelist *p = head;
//     struct siglelist *q = p->next;

//     while (q->next != NULL)
//     {
//         q = q->next;
//         p = p->next;
//     }
//     p->next = NULL;
//     q->next = NULL;
//     free(q);
//     q = NULL;
// }

// void list_show(struct siglelist *head)
// {

//     if (head == NULL) {
//         return;
//     }

//     list_show(head->next);
//     printf("%d\n", head->data);
// }

// int main()
// {
//     // 初始化链表的头结点
//     struct siglelist *myhead = list_init(56);

//     // 其他的节点往头结点代表的链表后面插入
//     list_insert_tail(97, myhead);
//     list_insert_tail(714, myhead);
//     list_insert_tail(89, myhead);
//     list_insert_tail(532, myhead);

//     printf("栈顶\n");
//     list_show(myhead);
//     printf("栈底\n");

//     printf("=======删除后========\n");

//     delete(myhead);

//     printf("栈顶\n");
//     list_show(myhead);
//     printf("栈底\n");
//     return 0;
// }

struct stack
{
    int data;
    int size;
    struct stack *next;
    struct stack *stacktop;
};

// 初始化头节点，但是不存放值
struct stack *stack_init()
{
    struct stack *head = malloc(sizeof(struct stack));
    head->next = NULL;
    head->size = 0;
    head->stacktop = head;
    return head;
}

struct stack *input(struct stack *head, int newdata)
{
    // 增加新节点并初始化
    struct stack *newnode = malloc(sizeof(struct stack));
    newnode->next = NULL;
    newnode->size = 0;
    newnode->stacktop = head;

    head->stacktop->next = newnode;
    head->stacktop = newnode;
    head->size++;
    return head;
}

struct stack *output(struct stack *head)
{

    if (head->size == 0)
    {
        printf("没有元素，无法出栈！\n");
        return head;
    }

    int temp = head->stacktop->data;
    struct stack *p = head;

    while (p->next != head->stacktop)
    {
        p = p->next;
    }
    p->next = NULL;
    free(head->stacktop);

    head->size--;
    printf("%d出栈\n", temp);

    head->stacktop = p;

    return head;
}

int main(int argc, char const *argv[])
{
    struct stack *myhead = stack_init();

    myhead = input(myhead, 12);
    myhead = input(myhead, 13);
    myhead = input(myhead, 14);
    myhead = input(myhead, 15);
    myhead = input(myhead, 16);
    printf("%d\n", myhead->size);

    static int n = 1;
    while (myhead->size != 0)
    {
        myhead = output(myhead);
        printf("出栈%d个元素\n", n);
        printf("还剩%d个元素\n", myhead->size);
        printf("\n");
    }
    return 0;
}
