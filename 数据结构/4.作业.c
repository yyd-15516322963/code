#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct game_info
{
    char name[20];
    int blood;
    int attack;
};

struct list_circle
{
    struct game_info data;
    struct list_circle *next;
};

struct list_circle *list_init(char *name, int blood, int attack)
{
    struct list_circle *headnode = malloc(sizeof(struct list_circle));
    headnode->next = NULL;
    strcpy(headnode->data.name, name);
    headnode->data.attack = attack;
    headnode->data.blood = blood;

    headnode->next = headnode;
    return headnode;
}

void list_insert_tail(const char *name, int blood, int attack, struct list_circle *head)
{
    // 准备新的节点
    struct list_circle *newnode = malloc(sizeof(struct list_circle));
    newnode->next = NULL;
    strcpy(newnode->data.name, name);
    newnode->data.attack = attack;
    newnode->data.blood = blood;

    struct list_circle *p = head;

    while (p->next != head)
    {
        p = p->next;
    }

    newnode->next = head;
    p->next = newnode;
}

// 删除
struct list_circle *delete(char *delname, struct list_circle *head)
{
    if (head == NULL) {
        printf("链表为空，无法删除！\n");
        return head;
    }
    struct list_circle *p = head;
    struct list_circle *q = p->next; 

    // 要删除的是头节点
    if (strcmp(p->data.name, delname) == 0)
    {
        // 找到尾节点
        struct list_circle *tail = head;
        while (tail->next != head)
        {
            tail = tail->next;
        }
        head = p->next;
        tail->next = head;

        free(p);
        return head;
    }

    // 要删除的是中间或尾部节点
    while (q != head)
    {
        // 如果下一个节点 q 是要删除的节点
        if (strcmp(q->data.name, delname) == 0)
        {
            p->next = q->next;

            free(q);
            return head;
        }
        // 如果 q 不是要删除的节点，就同时移动 p 和 q
        p = p->next;
        q = q->next;
    }

    // 4. 如果循环结束都没有找到
    printf("没有找到名字为 '%s' 的节点！\n", delname);
    return head;
}

void list_show(struct list_circle *head)
{

    // 使用一个临时指针 p 来遍历，避免修改头指针 head
    struct list_circle *p = head;
    // 当 p 不为 NULL 时，说明还有节点可以访问
    while (p->next != head)
    {
        printf("========人物信息=====\n");
        printf("名字 %s\n", p->data.name);
        printf("血量 %d\n", p->data.blood);
        printf("攻击力 %d\n", p->data.attack);
        p = p->next; // 移动到下一个节点
    }
    printf("========个人物信息=====\n");
    printf("名字 %s\n", p->data.name);
    printf("血量 %d\n", p->data.blood);
    printf("攻击力 %d\n", p->data.attack);
}

int main(int argc, char const *argv[])
{
    struct list_circle *myhead = list_init("aaa", 3000, 110);

    list_insert_tail("bbb", 4000, 100, myhead);
    list_insert_tail("ccc", 5000, 80, myhead);
    list_insert_tail("ddd", 2000, 200, myhead);
    list_insert_tail("eee", 2500, 120, myhead);

    list_show(myhead);

    myhead = delete("eee", myhead);

    printf("========================修改后====================\n");
    list_show(myhead);
    return 0;
}
