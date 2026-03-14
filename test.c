#include <stdio.h>
#include <math.h>

int is_prime(int n)
{
    if (n == 1)
        return 0;
    if (n == 2)
        return 1;
    if (n % 2 == 0)
        return 0;
    for (int i = 3; i <= sqrt(n); i += 2)
    {
        if (n % i == 0)
            return 0;
    }
    return 1;
}

int func(int num)
{
    int k1, k2;
    int flag = 0;
    int num1 = 2;
    int num2;
    while (num1 <= (num / 2))
    {
        num2 = num - num1;
        if (is_prime(num1) && is_prime(num2))
        {
            printf("%d = %d + %d\n", num, num1, num2);
            flag = 1;
        }
        num1++;
    }
    if (flag == 0)
    {
        printf("%d不能有两个素数组成！\n", num);
    }
}

int main(int argc, char const *argv[])
{
    func(34);
    return 0;
}
