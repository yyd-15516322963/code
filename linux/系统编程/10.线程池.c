#include "myhead.h"

struct threadpool
{
    int threadnum;
    pthread_t *id;
    int tasknum;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

struct threadpool *pool_init(int n)
{
    struct threadpool *pool = malloc(sizeof(struct threadpool));
    pool->id = malloc(n * sizeof(pthread_t));
    pthread_mutex_init(&(pool->mutex), NULL);
    pthread_cond_init(&(pool->cond), NULL);
}

struct tasklist
{
    void *(*taskp)(void *);
    void *taskarg;
    struct tasklist *next;
};

struct tasklist *init()
{
    struct tasklist *headnode = malloc(sizeof(struct tasklist));
    headnode->taskarg = NULL;
    headnode->taskp = NULL;
    headnode->next = NULL;
    return headnode;
}

void tasklist_tail(void *(*taskp)(void *), void *taskarg, struct tasklist *head)
{
    struct tasklist *newnode = malloc(sizeof(struct tasklist));
    newnode->taskarg = taskarg;
    newnode->taskp = taskp;
    newnode->next = NULL;

    struct tasklist *p = head;
    while (p->next != NULL)
    {
        p = p->next;
    }

    p->next = newnode;
}

void tasklist_show(struct tasklist *head)
{
    struct tasklist *p = head->next;
    while (p != NULL)
    {
        (p->taskp)(p->taskarg);
        p = p->next;
    }
}

struct addnum
{
    int n1;
    int n2;
};

void *add(void *arg)
{
    struct addnum *p = (struct addnum *)arg;
    printf("两个数的和是:%d\n", p->n1 + p->n2);
    return NULL;
}

void *show_bmp(void *bmppath)
{
    printf("bmp图片的路径名是:%s\n", (char *)bmppath);
    return NULL;
}

int main(int argc, char const *argv[])
{
    struct tasklist *myhead = init();

    struct addnum arg = {10, 20};
    tasklist_tail(add, &arg, myhead);

    tasklist_tail(show_bmp, "/1.bmp", myhead);

    tasklist_show(myhead);

    return 0;
}
