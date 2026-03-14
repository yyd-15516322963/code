#include "myhead.h"

int main(int argc, char const *argv[])
{
    int ret = fork();
    while (1)
    {
        if (ret > 0)
        {
            int n, i = 1;
            int lcdfd;
            int bmpfd;
            // 修改1：扩大bmp_buf到一行的大小（2400字节），解决缓冲区越界
            char bmp_buf[800 * 3] = {0};
            char lcd_buf[4] = {0};

            memset(bmp_buf, 0, sizeof(bmp_buf));
            memset(lcd_buf, 0, sizeof(lcd_buf));

            // 打开液晶屏的驱动
            lcdfd = open("/dev/fb0", O_RDWR);
            if (lcdfd == -1)
            {
                perror("打开液晶屏失败了!\n");
                return -1;
            }

            // 打开24位bmp图片
            bmpfd = open("/picture.bmp", O_RDWR);
            if (bmpfd == -1)
            {
                perror("打开图片失败了!\n");
                close(lcdfd);
                return -1;
            }

            // 跳过BMP文件头54字节
            lseek(bmpfd, 54, SEEK_SET);

            while (1)
            {
                // 修改2：提前判断i>480，避免无效循环
                if (i > 480)
                    break;

                // 修改3：修正LCD偏移（定位到当前行开头，避免越界）
                lseek(lcdfd, 800 * 4 * (480 - i), SEEK_SET);

                // 读取一行BMP像素（800*3=2400字节）
                n = read(bmpfd, bmp_buf, sizeof(bmp_buf));
                if (n <= 0)
                {
                    printf("读取完成\n");
                    break;
                }

                // 逐像素写入LCD（800个）
                for (int j = 0; j < 800 && j * 3 < n; j++)
                {
                    lcd_buf[0] = bmp_buf[j * 3];
                    lcd_buf[1] = bmp_buf[j * 3 + 1];
                    lcd_buf[2] = bmp_buf[j * 3 + 2];
                    lcd_buf[3] = 0x00;
                    // 修改4：写入前判断是否超出显存，避免段错误
                    if ((i - 1) * 800 + j < 800 * 480)
                    {
                        write(lcdfd, lcd_buf, 4);
                    }
                }

                // // 跳过BMP行尾填充字节
                // lseek(bmpfd, ((800 * 3 + 3) / 4 * 4) - 800 * 3, SEEK_CUR);
                i++;
            }

            // 关闭文件
            close(lcdfd);
            close(bmpfd);
            printf("这是进程1\n");
        }
        else if (ret == 0)
        {
            printf("这是进程2\n");
            system("madplay ./beyond.mp3");
        }
        else
        {
            printf("创建进程失败\n");
        }
    }

    return 0;
}
