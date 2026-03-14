#include <stdio.h>

int main(int argc, char const *argv[])
{
    
#ifdef _WIN32
    printf("WIN\n");

#elif defined (__linux__)
    printf("LINUX\n");

#else
    printf("others\n");
#endif


#ifdef DEBUG
    printf("DEBUG\n");
#endif
    return 0;
}
