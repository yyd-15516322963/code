#include <stdio.h>

void arr_p(int **q1, int len1)
{
    for (int i = 0; i < len1; i++)
    {
        printf("%d\n", *q1[i]);
    }
}

int main()
{

    int arr[][4] = {1, 2, 3, 4,
                    5, 6, 7, 8};
    int len1 = sizeof(arr) / sizeof(arr[0]);
    int *p2 = arr[1];
    int *p1 = arr[0];
    int *p3 = arr[0];

    arr_p(&p1, len1);
}