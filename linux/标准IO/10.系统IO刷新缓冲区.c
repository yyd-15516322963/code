#include "myhead.h"

int main(int argc, char const *argv[])
{
    int fd;
    fd = open("./1.txt", O_RDWR);
    if (fd == -1)
    {
        printf("创建文件失败！\n");
    }

    int i = 0;
    while (1)
    {
        write(fd, "abc", 3);
        i++;
        if (i >= 8)
            break;
    }

    return 0;
}
