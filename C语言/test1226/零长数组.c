#include <stdio.h>
#include <stdlib.h>

struct demo
{
    char a;
    char b;
    int c;
    int arr[];
};

typedef struct demo1
{
    char a;
    char b;
    int c;
    int arr1[];
} Demo1;

typedef struct demo2
{
    char a;
    char b;
    int c;
    int *arr2;
} Demo2;

int main(int argc, char const *argv[])
{

    struct demo *p = malloc(sizeof(struct demo) + 100);

    p->a = 'a';
    p->b = 'b';
    p->c = 12;

    printf("%p \n", &(p->a));
    printf("%p \n", &(p->b));
    printf("%p \n", &(p->c));
    printf("%p \n", p->arr);

    int len;
    Demo1 *q1 = malloc(sizeof(Demo1) + sizeof(char) * len);

    for (int i = 0; i < len; i++)
    {
        q1->arr1[i] = i;
    }
    free(q1);
    q1 = NULL;

    // 两次malloc不好用,得释放两次，而且释放还得保证顺序正确
    // Demo2 *q2 = malloc(sizeof(Demo2));
    // q2->arr2 = sizeof(char) * len;

    // 两次malloc的方法必须
    // 先回收buf再回首结构体
    // free(q2->arr2);
    // free(q2);
    // q2->arr2 = NULL;
    // q2 = NULL;

    return 0;
}
