#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <sys/ioctl.h>

#define TEST_MAGIC 'x'                    //定义幻数

#define LED1 _IO(TEST_MAGIC, 0)   //_IO(TEST_MAGIC, 0)是linux内核驱动提供的led1的定义,后面学习驱动大家认真学习就能看懂        
#define LED2 _IO(TEST_MAGIC, 1)
#define LED3 _IO(TEST_MAGIC, 2)
#define LED4 _IO(TEST_MAGIC, 3) 

#define LED_ON  	0	//灯亮
#define LED_OFF		1   //灯灭


int main(int argc, char const *argv[])
{
    int fd = open("/dev/Led",O_RDWR);
    if(fd == -1)
    {
        perror("打开失败！\n");
        return -1;
    }

    while (1)
    {

        ioctl(fd, LED1, LED_ON);
        sleep(1);
        ioctl(fd, LED2, LED_ON);
        sleep(1);
        ioctl(fd, LED3, LED_ON);
        sleep(1);
        ioctl(fd, LED4, LED_ON);
        sleep(1);

        ioctl(fd, LED1, LED_OFF);
        ioctl(fd, LED2, LED_OFF);
        ioctl(fd, LED3, LED_OFF);
        ioctl(fd, LED4, LED_OFF);
        sleep(1);

    }

    close(fd);
    
    return 0;
}
