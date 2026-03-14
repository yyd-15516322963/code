#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    int fd1, fd2, fd3;
    char buf1[100] = {0};
    fd1 = open("./1.txt", O_RDWR);
    if (fd1 == -1)
    {
        printf("创建文件失败！\n");
    }

    read(fd1, buf1, 6);
    printf("UTF-8文件内容是：%s\n", buf1);

    close(fd1);

    char buf2[100] = {0};
    fd2 = open("./2.txt", O_RDWR);
    if (fd2 == -1)
    {
        printf("创建文件失败！\n");
    }

    read(fd2, buf2, 6);
    printf("Unicode文件内容是：%s\n", buf2);

    close(fd2);

    char buf3[100] = {0};
    fd3 = open("./3.txt", O_RDWR);
    if (fd3 == -1)
    {
        printf("创建文件失败！\n");
    }

    read(fd3, buf3, 6);
    printf("GBK文件内容是：%s\n", buf3);

    close(fd3);
    return 0;
}
