#include "myhead.h"

int main(int argc, char const *argv[])
{
    int k = 5, n = 0;
    do
    {
        switch (k)
        {
        case 1:
        case 3:
            n += 1;
            break;
        default:
            n = 0;
            k--;
        case 2:
        case 4:
            n += 2;
            k--;
            break;
        }
        printf("%d", n);
    } while (k > 0 && n < 5);
}