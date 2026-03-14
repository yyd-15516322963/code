#include <stdio.h>

int main(int argc, char const *argv[])
{

    int arr[10]; // 声明一个数组的时候他表示整个数组

    printf("%ld \n", sizeof(arr)); // 使用sizeof获取数组字节大小时,arr表示整个数组

    &arr; // 给数组取地址时表示整个数组,表示整个数组的地址

    // 除了以上三种情况,其他都表示首元素的地址 arr == &(arr[0])

    // & 取地址符: 返回一个变量/常量的内存地址
    // * 解引用符号: 根据内存地址找到那个变量/常量
    // 这两个符号是一对,两者可以互相抵消

    int i;
    char c;
    double d;

    printf("%p => %p\n", &i, &i + 1);
    printf("%p => %p\n", &c, &c + 1);
    printf("%p => %p\n", &d, &d + 1);

    int arr1[4];

    printf("%p => %p\n", arr1, arr1 + 1);   //+4.arr表示首元素地址，这里偏移首元素大小的地址
    printf("%p => %p\n", &arr1, &arr1 + 1); //+16.&arr表示整个数组的地址大小，这里偏移一个数组大小的地址

    printf("%d \n", arr1[0]);
    printf("%d \n", *(arr1 + 0));
    printf("%d \n", *(0 + arr1));
    printf("%d \n", *arr1);
    printf("%d \n", 0 [arr1]);

    return 0;
}
