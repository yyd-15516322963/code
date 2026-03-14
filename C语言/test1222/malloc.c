#include <stdio.h>
#include <stdlib.h>

// 相比栈内存，堆的总大小仅受限于物理内存，在物理内存允许的范围内，系统对堆内存的申请不做限制。
// 相比栈内存，堆内存从下往上增长。
// 堆内存是匿名的，只能由指针来访问。
// 自定义分配的堆内存，除非开发者主动释放，否则永不释放，直到程序退出。

int main(int argc, char const *argv[])
{
    double *d = malloc(sizeof(double));
    *d = 17.3;
    printf("%f \n", *d);

    int len = 10;
    // malloc(size)申请堆内存, size申请的堆内存字节大小
    // 申请长度为int [10]的堆内存
    int *p = malloc(sizeof(int) * 10);

    // calloc不仅申请指定大小count * size的堆空间,还会将其内存清零
    // calloc(count,size)
    int *p2 = calloc(10,sizeof(int));

    for (int i = 0; i < 10; i++)
    {
        // p[i] = i + 1;
        printf("p %d  p2 %d \n", p[i], p2[i]);
    }

    // 用完堆空间记得释放,传入堆空间的地址
    free(p);
    free(p2);
    // 回收完指针就变成了野指针,将其变为空指针
    p = NULL;
    p2 = NULL;


    return 0;
}
