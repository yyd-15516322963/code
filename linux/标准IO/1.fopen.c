#include "myhead.h"

int main(int argc, char const *argv[])
{
    FILE *myfile = fopen("./1.txt","w+");

    if(myfile == NULL)
    {
        perror("文件打开失败了！\n");
        return -1;
    }

    fclose(myfile);
    return 0;
}
