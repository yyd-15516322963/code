#include "myhead.h"

int main(int argc, char const *argv[])
{
    int exist = 0;
    char buf[100];
    char arr[100];
    char arr_cpy[100];
    FILE *myfile = fopen("./zuoye.txt", "r+");
    if (myfile == NULL)
    {
        perror("文件打开失败！\n");
        return -1;
    }
    scanf("%s", arr);
    strcpy(arr_cpy, arr);
    char *p1 = strtok(arr, "@");
    fseek(myfile, 0, SEEK_SET);

    while ((fgets(buf, 100, myfile)) != NULL)
    {
        // buf[strcspn(buf, "\n")] = '\0';
        char *p2 = strtok(buf, "@");
        if (p2 != NULL && strcmp(p1, p2) == 0)
        {
            exist = 1;
            break;
        }
        memset(buf, 0, sizeof(buf));
    }

    if (exist == 1)
    {
        perror("该用户已存在！\n");
    }
    else
    {
        fwrite(arr_cpy, 1, strlen(arr_cpy), myfile);
        fwrite("\n", 1, 1, myfile);
        perror("写入成功！\n");
    }

    fclose(myfile);

    return 0;
}
