#include "myhead.h"

int main(int argc, char const *argv[])
{
    FILE *myfile = fopen("./1.txt", "r+");

    if (myfile == NULL)
    {
        perror("文件打开失败了！\n");
        return -1;
    }
    int i = 0;

    while (1)
    {
        fwrite("hello", 1, 5, myfile);
        i++;
        if (i >= 8)
        {
            return 0;
        }
    }

    return 0;
}
