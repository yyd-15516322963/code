#include "myhead.h"

int main(int argc, char const *argv[])
{
    char buf[1000];
    memset(buf, 0, sizeof(buf));
    FILE *myfile = fopen("./1.txt", "r");

    if (myfile == NULL)
    {
        perror("文件打开失败了！\n");
        return -1;
    }
    fgets(buf, 100, myfile);
    printf("%s\n", buf);

    fgets(buf, 100, myfile);
    printf("%s\n", buf);

    fgets(buf, 100, myfile);
    printf("%s\n", buf);

    fclose(myfile);
    return 0;
}
