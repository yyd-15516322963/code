#include <stdio.h>

int main(int argc, char const *argv[])
{

    int len;

    scanf("%d", &len);

    int arr[len];
    
    printf("len = %d\n", len);

    for (int i = 0; i < len; i++)
    {
        arr[i] = i * 2;
        printf("%d\n", arr[i]);
    }
    return 0;
}
