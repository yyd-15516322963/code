#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <sys/mman.h>

int main(int argc, char const *argv[])
{
    int red = 0x00ff0000;
    int *lcdmem;

    int lcdfd = open("/dev/fb0", O_RDWR);
    if (lcdfd == -1)
    {
        printf("打开文件失败！\n");
    }

    lcdmem = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcdfd, 0);

    if (lcdmem == NULL)
    {
        perror("映射失败！\n");
        return -1;
    }

    for (int i = 0; i < 800 * 480; i++)
    {
        *(lcdmem + i) = red;
    }

    close(lcdfd);

    munmap(lcdmem, 800 * 480 * 4);
    return 0;
}
