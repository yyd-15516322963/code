#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>

int main(int argc, char const *argv[])
{
    int red = 0x00ff0000;
    int white = 0x00ffffff;
    int blue = 0x000000ff;

    int lcdfd = open("/dev/fb0", O_RDWR);
    if (lcdfd == -1)
    {
        printf("打开文件失败！\n");
    }

    for (int i = 0; i < 480; i++)
    {
        for (int j = 0; j < 266; j++)
        {
            write(lcdfd, &blue, 4);
        }
        for (int j = 0; j < 268; j++)
        {
            write(lcdfd, &white, 4);
        }
        for (int j = 0; j < 266; j++)
        {
            write(lcdfd, &red, 4);
        }
    }

    close(lcdfd);
    return 0;
}
