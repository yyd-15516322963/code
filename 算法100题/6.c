// 题目：用*号输出字母C的图案。 
// 1.程序分析：可先用'*'号在纸上写出字母C，再分行输出。 
#include <stdio.h>

int main(int argc, char const *argv[])
{
    for (int i = 0; i < 5; i++)
    {
        if(i == 0 || i == 4)
        {
            printf("*******\n");
        }else
        printf("*\n");
    }
    
    return 0;
}
