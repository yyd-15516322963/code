#include "myhead.h"

int main(int argc, char const *argv[])
{
    char buf[100];
    int ret;
    FILE *myfile1 = fopen("./test.txt", "r");
    if (myfile1 == NULL)
    {
        perror("文件打开失败！\n");
        fclose(myfile1);
        return -1;
    }

    FILE *myfile2 = fopen("./test2.txt", "w+");
    if (myfile2 == NULL)
    {
        perror("文件打开失败！\n");
        fclose(myfile2);
        return -1;
    }

    while (1)
    {
        memset(buf, 0, sizeof(buf));
        if (feof(myfile1))
        {
            perror("读取完毕！\n");
            break;
        }
        ret = fread(buf, 1, sizeof(buf), myfile1);
        fwrite(buf, 1, ret, myfile2);
    }

    fclose(myfile1);
    fclose(myfile2);

    return 0;
}
