#include "myhead.h"

int main(int argc, char const *argv[])
{
    pid_t id1,id2;
    id1 = fork();
    while (1)
    {
        if (id1 > 0)
    {
        printf("父进程!\n");
        sleep(1);
        id2 = fork();
        if (id2 == 0)
        {
            printf("第二个子进程!\n");
            sleep(1);
        }
    }
    else if (id1 == 0)
    {
        printf("第一个子进程！\n");
        sleep(1);
    }
    }
    
    
    return 0;
}
