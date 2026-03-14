#include "myhead.h"

int main(int argc, char const *argv[])
{

    int n;
    FILE *myfile1 = fopen("./1.txt", "r+");

    if (myfile1 == NULL)
    {
        perror("文件打开失败了！\n");
        return -1;
    }
    FILE *myfile2 = fopen("./2.txt", "w+");

    if (myfile2 == NULL)
    {
        perror("文件打开失败了！\n");
        return -1;
    }

    while (1)
    {
        n = fgetc(myfile1);
        if(feof(myfile1) == 1)
            break;
        fputc(n, myfile2);
    }

    fclose(myfile1);
    fclose(myfile2);

    return 0;
}
