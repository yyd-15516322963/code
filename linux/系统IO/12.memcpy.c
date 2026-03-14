#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>

// void *memcpy(void dest[restrict .n], const void src[restrict .n],size_t n);

int main(int argc, char const *argv[])
{
    int n = 90;
    double n2 = 89.9;

    void *p = malloc(100);
    // memcpy(p, &n, sizeof(int));
    memcpy(p, &n2, sizeof(double));

    // printf("%d\n", *((int *)p));
    printf("%lf\n", *((double *)p));

    free(p);
    return 0;
}
