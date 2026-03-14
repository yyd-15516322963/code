#include <stdio.h>

#define IS_EVEN(a) (a) % 2 == 0
#define PRINTF_EVEN(num)                \
    do                                  \
    {                                   \
        for (int i = 0; i < (num); i++) \
        {                               \
            if (IS_EVEN(i))             \
            {                           \
                printf("%d ", i);       \
            }                           \
        }                               \
        printf("\n");                   \
    } while (0)

#define SWAP(a, b, temp) \
    do                   \
    {                    \
        (temp) = (a);    \
        (a) = (b);       \
        (b) = (temp);    \
    } while (0)

int main(int argc, char const *argv[])
{
    /*
    int num = 20;
    do
    {
      for (int i = 0; i < num; i++)
      {
        if (i % 2 == 0)
        {
            printf("%d ", i);
        }
        printf("\n");
      }
    } while (0);
    */
    PRINTF_EVEN(30);

    int a = 2, b = 9, t;
    SWAP(a, b, t);

    printf("a = %d b = %d \n", a, b);
    return 0;
}
