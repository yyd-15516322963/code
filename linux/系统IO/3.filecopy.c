#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    int fd1, fd2, n;
    char buf1[1000] = {0};

    fd1 = open("/mnt/d/code/share/1笔记.txt", O_RDWR);
    if (fd1 == -1)
    {
        printf("文件打开失败了\n");
    }

    fd2 = open("/mnt/d/code/share/2笔记.txt", O_CREAT | O_EXCL | O_RDWR, 0777);
    if (fd2 == -1)
    {
        printf("文件创建失败了\n");
    }

    while (1)
    {
        memset(buf1, 0, 1000);
        n = read(fd1, buf1, 1000);
        if (n == 0)
        {
            printf("读取完成\n");
            break;
        }
        write(fd2, buf1, n);
    }

    close(fd1);
    close(fd2);
    return 0;
}
