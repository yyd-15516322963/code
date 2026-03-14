#include <stdio.h>

int main(int argc, char const *argv[])
{
    // int [19]是他的数据类型(去掉变量名即可)
    int arr[19] = {1,2,3};

    // 二维数组定义
    // int [19] arr[10],改写为下面的方式，将[19]放在最后
    int arr1[][3] = {
        // 前边[]中数字可以省略，他表示arr1数组的长度
        // 后边数字不可以省略，他表示int[3]数据类型
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9},

    };
    return 0;
}
