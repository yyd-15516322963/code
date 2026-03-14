#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> //read的头文件
#include <strings.h>

int main(int argc, char const *argv[])
{

    if(access(argv[1],F_OK) == -1)
    {
        printf("文件不存在\n");
        return -1;
    }
    struct stat mystat;
    stat(argv[1], &mystat);

    if ((mystat.st_mode & S_IRUSR))
    {
        printf("bu需要修改\n");
    }
    else
    {
        printf("需要修改\n");
        chmod(argv[1], 0777);
    }

    return 0;
}
