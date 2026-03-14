#include <stdio.h>

int main(int argc, char const *argv[])
{
    // 一维数组
    int arr[3] = {11, 22, 33};
    // arr 除了sizeof/取地址/声明外都是首元素地址
    // 首元素是int型,声明一个int型地址变量, int *型接收首元素的地址
    int *p = arr;
    // 访问它就是利用地址偏移运算配合解引用
    printf("arr[0] %d \n", *p);
    printf("arr[0] %d \n", *(p + 0));
    printf("arr[0] %d \n", p[0]);

    printf("arr[0] %d \n", *arr);
    printf("arr[0] %d \n", *(arr + 0));
    printf("arr[0] %d \n", arr[0]);

    // 访问下一个元素,使用地址配个加法运算实现地址的偏移
    printf("arr[1] %d \n", *(p + 1));
    printf("arr[1] %d \n", p[1]);

    printf("arr[1] %d \n", *(arr + 1));
    printf("arr[1] %d \n", arr[1]);

    printf("\n");

    // 其实我们上面的指针并不指向数组本身,而是指向数组的首元素地址

    // 我们声明一个指向int arr[3] 整体的指针
    // 首先先确定数组自身的类型int [3], 我们对应的指针类型 int (*)[3]
    int (*arr_p)[3] = &arr; // arr_p是一个指针,指向 int [3] 型的数组本身的地址
                            // int *a[3]; a是一个数组长度为3内部成员是 int * 地址


    printf("arr[0] %d \n", (*(&arr))[0]); // *&arr[0]
    printf("arr[0] %d \n", (*(&arr + 0))[0]);
    printf("arr[0] %d \n", ((&arr)[0][0]));
    // 因为 arr_p == &arr
    printf("arr[0] %d \n", (*arr_p)[0]); // *&arr[0]
    printf("arr[0] %d \n", (*(arr_p + 0))[0]);
    printf("arr[0] %d \n", arr_p[0][0]);

    printf("arr[1] %d \n", (arr_p[0][1]));
    printf("arr[1] %d \n", ((*arr_p)[1]));

    printf("arr[1] %d \n", ((&arr)[0][1]));
    printf("arr[1] %d \n", ((*&arr)[1]));
    return 0;
}
