#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 
#include <strings.h>

int main(int argc, char const *argv[])
{
    int fd;
    char buf[10] = {0};
    fd = open(argv[1], O_RDWR);
    if (fd == -1)
    {
        printf("打开文件失败！\n");
        return -1;
    }

    // lseek(fd, -10, SEEK_SET);
    // lseek(fd, 10, SEEK_SET);
    // lseek(fd, -10, SEEK_CUR);
    // lseek(fd, 10, SEEK_CUR);
    // lseek(fd, -10, SEEK_END);
    // lseek(fd, 10, SEEK_END);
    // read(fd, buf, 2);
    // printf("读到的是：%s\n", buf);

    // write(fd, "oo", 2);
    // printf("结果是：%s\n", buf);

    write(fd,"he",2);

    read(fd, buf, 3);
    printf("%s\n", buf);

    lseek(fd, -4, SEEK_CUR);
    bzero(buf,10);
    read(fd, buf, 5);
    printf("%s\n",buf);

    close(fd);
    return 0;
}
