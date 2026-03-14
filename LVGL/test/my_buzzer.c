#include "myhead.h"

void fun()
{
    // 打开蜂鸣器设备
    int fd = open("/dev/beep", O_RDWR);
    if(fd == -1) {
        perror("open beep device failed");
        return; // 改用return，避免直接exit导致程序退出
    }

    // 1. 使能蜂鸣器（响）
    int ret = ioctl(fd, ON, 1);
    printf("付款成功 \n");
    if(ret == -1) {
        perror("ioctl ON failed");
        close(fd); // 失败时先关闭设备再返回
        return;
    }

    // 2. 保持响1秒
    sleep(1);

    // 3. 关闭蜂鸣器（停）
    ret = ioctl(fd, OFF, 1);
    if(ret == -1) {
        perror("ioctl OFF failed");
    }

    // 4. 关闭设备文件描述符
    close(fd);
}