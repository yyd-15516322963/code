#include <stdio.h>

int array(int *arr, int len1, int len2)
{
    for (int i = 0; i < len1; i++)
    {
        for (int j = 0; j < len2; j++)
        {
            printf("%d ", arr[i * len1 + j]);
        }
        printf("\n");
    }
}
int main(int argc, char const *argv[])
{
    int arr[][3] = {{1, 2, 3}, {4, 5, 6}};
    int len1 = sizeof(arr) / sizeof(arr[0]);
    int len2 = sizeof(arr[0]) / sizeof(arr[0][0]);

    array((int *)arr, len1, len2);

    return 0;
}
