#include <stdio.h>

int main(int argc, char const *argv[])
{
    // int arr[][3] = {{1,2,3},{4,5,6}};

    // int **p[3] = &arr;
    // return 0;
    int arr1[4] = {1, 2, 3, 4};
    int (*p1)[4] = &arr1;

    int arr2[2][4] = {{1, 2, 3, 4},
                     {5, 6, 7, 8}};
    int (*p2)[2][4] = &arr2;
    int (*p3)[4] = arr2;
    
}
