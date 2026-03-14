#include <stdio.h>

int main(int argc, char const *argv[])
{
    int arr[4][5] = {
        {11, 22, 33, 44, 55},
        {66, 77, 88, 99, 10},
        {3, 4, 5, 6, 7},
        {12, 13, 14, 15, 16}};


        printf("%d\n",arr[1][0]);
        printf("%d\n",*(arr + 1)[0]);
        printf("%d\n",*(*(arr + 1) + 0));
        printf("%d\n",*(*(arr + 1)));
        printf("%d\n",*(arr[1]));

        printf("%d\n",arr[0][5]);
        printf("%d\n",*(*(arr) + 5));
        printf("%d\n",*(arr[0] + 5));

    return 0;
}
