#include "myhead.h"

int main(int argc, char const *argv[])
{
    DIR *mydir = opendir("/mnt/d/code/linux");
    if (mydir == NULL)
    {
        perror("文件打开失败！\n");
        return -1;
    }

    struct dirent *myread;

        while ((myread = readdir(mydir)) != NULL)
        {
            if (myread->d_type == DT_DIR)
            {
                printf("文件%s是目录！\n", myread->d_name);
            }
        }
    closedir(mydir);
    return 0;
}
