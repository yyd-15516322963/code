#include "myhead.h"

int main(int argc, char const *argv[])
{
    struct input_event myevent;

    int fd = open("/dev/input/event0", O_RDWR);

    if (fd == -1)
    {
        perror("文件打开失败！\n");
        return -1;
    }

    while (1)
    {
        read(fd, &myevent, sizeof(myevent));

        if (myevent.type == EV_ABS) // 触摸屏事件
        {
            if (myevent.code == ABS_X)
            {
                printf("x坐标是：%d\n", (800 * myevent.value) / 1024);
            }
            if (myevent.code == ABS_Y)
            {
                printf("y坐标是：%d\n", (480 * myevent.value) / 600);
            }
        }
    }

    close(fd);

    return 0;
}
