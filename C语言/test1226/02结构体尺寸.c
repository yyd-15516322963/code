#include <stdio.h>

int main(int argc, char const *argv[])
{
    char c;
    int i;
    float f = 1.0;
    double d = 1.0;

    printf("%lu \n", &c);
    printf("%lu \n", &i);
    printf("%lu \n", &f);
    printf("%f \n", f);
    printf("%lu \n", &d);
    printf("%f \n", f);

    return 0;
}
