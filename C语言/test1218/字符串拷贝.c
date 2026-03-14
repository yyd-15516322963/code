#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    char str1[100] = {"hello world"};
    char cStr2[100];

    strcpy(cStr2,str1);
    // int i = 0,j = 0;
    // while(str1[i] != '\0')
    // { 
    //     if(str1[i] != 'l')
    //     {
    //         cStr2[j] = str1[i];
    //         j++;
    //     }
    //     i++;
    // }



    // for(int i = 0;i<100;i++)
    // {
    //     cStr2[i] = str1[i];
    //     if(str1[i] == '\0')
    //     {
    //         break;
    //     }
    // }
    // cStr2[j] = '\0';
    printf("%s \n",cStr2);
    return 0;
}
