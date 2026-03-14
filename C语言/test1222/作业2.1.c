#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    char arr[] = "hello";
    int len = strlen(arr);

    for (int i = 0; i < len / 2; i++)
    {
        char temp = arr[i];
        arr[i] = arr[len - 1 - i];
        arr[len-i-1] = temp;
    }
    printf("%s \n", arr);
    return 0;
}
