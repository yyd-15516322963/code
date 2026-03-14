#include "myhead.h"

int main()
{
    int ret;
    FILE *myfile = fopen("./1.txt","w+");
    if(myfile == NULL)
    {
        perror("文件打开失败！\n");
    }

    ret = fwrite("helloworld",2,3,myfile);
    printf("ret=%d\n", ret);
    return 0;
}