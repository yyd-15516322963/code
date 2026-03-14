#include "myhead.h"

/*
    无名管道实现父子进程通信
       父-->子
       int pipe(int fildes[2]);
            参数: fildes[0]   读端的文件描述符
                 fildes[1]   写端的文件描述符
        读端: 只能用于从无名管道读取数据,不能用于写入数据
        写端: 只能用于写入数据到无名管道,不能用于读取数据

    1.无名管道的特点
         (1)如果无名管道中没有数据,read会阻塞


*/

int main()
{
    pid_t id;
    int fd1[2];
    int fd2[2];
    int ret1, ret2;
    char sbuf1[100], sbuf2[100];

    // 第一步: 创建无名管道
    ret1 = pipe(fd1);
    if (ret1 == -1)
    {
        perror("创建无名管道1失败了!\n");
        return -1;
    }
    ret2 = pipe(fd2);
    if (ret2 == -1)
    {
        perror("创建无名管道2失败了!\n");
        return -1;
    }

    // 创建一个新的进程 -->子进程
    id = fork();
    if (id > 0) // 父进程
    {
        while (1)
        {
            bzero(sbuf1, 100);
            printf("请输入要发送给子进程的信息!\n");
            scanf("%s", sbuf1);
            if (strcmp(sbuf1, "quit") == 0)
            {
                exit(0);
            }
            // 写入到无名管道
            write(fd1[1], sbuf1, strlen(sbuf1));

            bzero(sbuf2, 100);
            // 读取无名管道中的数据
            read(fd2[0], sbuf2, 100); // 读取100个字节,如果无名管道没有100个字节的数据,有多少就读取多少
            printf("儿子给我发送的信息是: %s\n", sbuf2);
        }
    }
    else if (id == 0) // 子进程
    {
        while (1)
        {
            bzero(sbuf1, 100);
            // 读取无名管道中的数据
            read(fd1[0], sbuf1, 100); // 读取100个字节,如果无名管道没有100个字节的数据,有多少就读取多少
            printf("老爸给我发送的信息是: %s\n", sbuf1);

            bzero(sbuf2, 100);
            printf("请输入要发送给父进程的信息!\n");
            scanf("%s", sbuf2);
            // 写入到无名管道
            write(fd2[1], sbuf2, strlen(sbuf2));
            if (strcmp(sbuf2, "quit") == 0)
            {
                exit(0);
            }
        }
    }
    else
    {
        perror("fork函数调用失败!\n");
        return -1;
    }

    // 关闭无名管道
    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);
    return 0;
}