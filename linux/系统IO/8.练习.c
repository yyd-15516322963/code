#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>

int main(int argc, char const *argv[])
{
    char buf[100];
    int fd1 = open("./test.txt", O_RDWR);
    if (fd1 == -1)
    {
        printf("文件打开失败！\n");
    }

    int fd2 = open("./test2.txt", O_CREAT | O_RDWR);
    if (fd2 == -1)
    {
        printf("文件打开失败！\n");
    }
    read(fd1, buf, 2);

    write(fd2, buf, 2);

    write(fd2, "bt", 2);

    bzero(buf, 100);
    read(fd1, buf, 5);
    write(fd2, buf, 5);

    close(fd1);
    close(fd2);
    return 0;
}
