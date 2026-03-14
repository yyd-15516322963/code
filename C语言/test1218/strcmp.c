#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    char str1[100] = {"hello world"};
    char str2[25] = {"hello Worldasa"};

    // int ret = strcmp(str1,str2);//判断两个字符串是否相等，相等返回0，不同返回非0
    //原理
    int max = (strlen(str1)>strlen(str2))?strlen(str1):strlen(str2);
    int len = max;
    printf("%d\n",len);
    int ret;
    int i;
    for(int i = 0;i<len;i++)
    {
        if(str1[i] != str2[i])
        {
            ret = str1[i]- str2[i];
            break;
        }

    }
    printf("%d\n",ret);
    return 0;
}
