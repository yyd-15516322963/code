#include "myhead.h"

int main(int argc, char const *argv[])
{
    pid_t id1, id2;
    int ret;
    int state;
    printf("我是父进程，进程号是%d,我的父进程是%d\n", getpid(), getppid());

    id1 = fork();
    if (id1 > 0)
    {
        id2 = fork();
        if (id2 == 0)
        {
            for (int i = 0; i < 5; i++)
            {
                printf("我是第二个子进程，我的id号是%d,我的父进程是%d\n", getpid(), getppid());
                // sleep(1);
            }
            exit(2);
        }
    }
    else if (id1 == 0)
    {
        for (int i = 0; i < 5; i++)
        {
            printf("我是第一个子进程，我的id号是%d,我的父进程是%d\n", getpid(), getppid());
            // sleep(1);
        }
        exit(1);
    }

    ret = waitpid(id1, &state, 0);
    printf("我现在回收的进程id是%d\n", ret);
    ret = waitpid(id2, &state, 0);
    printf("我现在回收的进程id是%d\n", ret);
    return 0;
}
