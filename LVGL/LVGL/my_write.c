#include "myhead.h"

void my_write(char *buf)
{
    FILE *myfile = fopen("./user_info.txt","a+");
    if(myfile == NULL)
    {
        perror("文件打开失败！\n");
    }

    fwrite(buf,2,5,myfile);
    return 0;
}