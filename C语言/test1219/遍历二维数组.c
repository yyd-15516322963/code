#include <stdio.h>

int main(int argc, char const *argv[])
{
    int arr[4][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9},
        {65, 67, 3}};
    int row = sizeof(arr) / sizeof(arr[0]);
    int col = sizeof(arr[0]) / sizeof(arr[0][0]);

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }

    // 数组的内存是连续的,在内存中我们可以将其看做下面的结构
    int arr1[4][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {65, 67, 3}};

    printf("%p\n", &(arr1[1][0]));
    printf("%p\n", &(arr1[1][1]));

    printf("%d\n", arr1[1][0]);
    printf("%d\n", arr1[1][1]);

    return 0;
}
