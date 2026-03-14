#include <stdio.h>

int main()
{
    char a1 = 0xff;
    printf("%#hhx\n",a1 << 4);

    unsigned char a2 = 0x0f;
    printf("%#hhx\n",a2 << 4);

    char a3 = 0xf0;
    printf("%#hhx\n",a3 >> 4);

    unsigned char a4 = 0x0f;
    printf("%#hhx\n",a4 >> 4);

    return 0;
}