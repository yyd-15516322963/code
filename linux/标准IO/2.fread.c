#include "myhead.h"

int main(int argc, char const *argv[])
{
    char buf[20];
    int ret1, ret2;
    memset(buf, 0, 19);
    FILE *myfile = fopen("./1.txt", "r");

    if (myfile == NULL)
    {
        perror("文件打开失败了！\n");
        return -1;
    }
    // ret1 = fread(buf, 10, 1, myfile);

    ret2 = fread(buf, 100, 1, myfile);
    printf("%s\n", buf);
    printf("ret2=%d\n", ret2);

    fclose(myfile);
    return 0;
}
