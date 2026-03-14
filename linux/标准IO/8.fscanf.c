#include "myhead.h"

int main(int argc, char const *argv[])
{

    FILE *myfile = fopen("./1.txt", "r+");

    if (myfile == NULL)
    {
        perror("文件打开失败了！\n");
        return -1;
    }

    int year, month, day;
    
    fscanf(myfile,"%d-%d-%d",&year, &month, &day);
    printf("%d %d %d\n", year, month, day);

    return 0;
}
