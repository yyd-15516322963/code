#include <stdio.h>

int main(int argc, char const *argv[])
{
    // int arr[4] = {};//四个元素都不赋值
    // int arr1[] = {1,2,3,3};
    // int arr2[4] = {1,2};
    // int arr3[4] = {2};//只给第一个元素赋值
    // int arr2[] = {};

    int arr[] = {1, 2, 3, 4, 5, 6};//一般情况下arr表示数组首元素的地址  &(arr[0])

    printf("%ld\n", sizeof(arr));//整个数组的大小
    printf("%ld\n", sizeof(arr[0]));//数组首元素的大小

    int len = sizeof(arr) / sizeof(arr[0]);//数组的长度

    printf("%d\n", len);

    for (int i = 0; i < len; i++)
    {
        printf("%d\n",arr[i]);
    }

    return 0;
}
