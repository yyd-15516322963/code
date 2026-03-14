#include "myhead.h"

int main(int argc, char const *argv[])
{
    // char name[20], pwd[20];
    // 打开蜂鸣器
    int fd = open("/dev/beep", O_RDWR);
    if (fd == -1)
    {
        perror("open:");
        exit(0);
    }
    // scanf("%s", name);
    int n = strcmp(argv[1], "root");
    // scanf("%s", pwd);
    int m = strcmp(argv[2], "123456");

    if (n != 0 || m != 0)
    {
        // 使能蜂鸣器
        ioctl(fd, ON, 1);
        sleep(1);
    }
    ioctl(fd, OFF, 1);

    close(fd);
    return 0;
}
