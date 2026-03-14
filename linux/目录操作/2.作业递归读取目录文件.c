#include "myhead.h"

void rec_dir(char *pathname)
{
    char buf[256];
    struct dirent *myread;
    DIR *mydir = opendir(pathname);
    if (mydir == NULL)
    {
        perror("目录打开失败！");
        return;
    }
    while ((myread = readdir(mydir)) != NULL)
    {
        if ((strcmp(myread->d_name, ".") == 0 || (strcmp(myread->d_name, "..")) == 0))
            continue;

        snprintf(buf, sizeof(buf), "%s/%s", pathname, myread->d_name);
        if (myread->d_type == DT_DIR) 
        {
            rec_dir(buf);
            printf("\n");
        }
        else
        {
            printf("文件的绝对路径名是：%s\n", buf);
        }
    }
    closedir(mydir);
}

int main(int argc, char const *argv[])
{
    rec_dir("/mnt/d/code/linux");

    return 0;
}
