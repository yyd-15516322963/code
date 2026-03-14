#include <stdio.h>

// void sele_sort(int arr[], int n)
// {
//     for (int i = 0; i < n - 1; i++)
//     {
//         int k = i;
//         for (int j = i + 1; j < n; j++)
//         {
//             if (arr[j] < arr[k])
//             {
//                 k = j;
//             }
//         }
//         int temp = arr[i];
//         arr[i] = arr[k];
//         arr[k] = temp;
//     }
// }

// void bubble_sort(int arr[], int n)
// {
//     for (int i = n - 1; i > 0; i--)
//     {
//         for (int j = 0; j < i; j++)
//         {
//             if (arr[j] > arr[j + 1])
//             {
//                 int temp = arr[j];
//                 arr[j] = arr[j + 1];
//                 arr[j + 1] = temp;
//             }
//         }
//         printf("%d ", arr[n - 1 - i]);
//     }
//     printf("%d\n", arr[n - 1]);
// }

// void bubble_sort2(int *arr, int n)
// {
//     for (int i = 0; i < n - 1; i++)
//     {
//         for (int j = 0; j < n - i - 1; j++)
//         {
//             if (arr[j] > arr[j + 1])
//             {

//                 int temp = arr[j];
//                 arr[j] = arr[j + 1];
//                 arr[j + 1] = temp;
//             }
//         }
//     }
// }

// void insertionSort(int arr[], int n)
// {
//     int i;
//     for (i = 1; i < n; i++)
//     {
//         int base = arr[i], j = i - 1;
//         while (j >= 0 && arr[j] > base)
//         {
//             arr[j + 1] = arr[j];
//             j--;
//         }
//         arr[j + 1] = base;
//     }
//     for (i = 0; i < n; i++)
//     {
//         printf("%d ", arr[i]);
//     }
//     printf("\n");
// }

void swap(int arr[], int i, int j)
{
    int tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}
// 快排哨兵
void patition(int arr[], int left, int right)
{
    if (left >= right)
    {
        return;
    }

    int i = left;
    int j = right;
    while (i < j)
    {
        while (i < j && arr[j] >= arr[left])
        {
            j--;
        }
        while (i < j && arr[i] <= arr[left])
        {
            i++;
        }
        swap(arr, i, j);
    }
    swap(arr, i, left);
    patition(arr, left, i - 1);
    patition(arr, i + 1, right);
}

int main(int argc, char const *argv[])
{

    int arr[] = {2, 4, 1, 7, 5, 8, 4, 9};
    int n = sizeof(arr) / sizeof(arr[0]);
    patition(arr, 0, n - 1);

    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}
