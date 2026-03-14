#include <stdio.h>

int change(int **q1,int **q2)
{
    int *temp;
    temp = *q1;
    *q1 = *q2;
    *q2 = temp;
}

int main(int argc, char const *argv[])
{
    int a = 10,b = 20;

    int *p1 = &a;
    int *p2 = &b;

    change(&p1,&p2);
    printf("%d %d\n",*p1,*p2);
    return 0;
}
