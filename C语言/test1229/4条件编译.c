#include <stdio.h>

#define SWITCH 1

int main(int argc, char const *argv[])
{

#if SWITCH == 0
    {
        printf("SWITCH == 0");
    }
#elif (SWITCH == 1)
    {
        printf("SWITCH == 1\n");
    }
#else
    {
        printf("SWITCH == 2");
    }

#endif
    return 0;
}
