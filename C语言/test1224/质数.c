#include <stdio.h>
#include <math.h>

int isPrime(int num) {

    // 逻辑代码, 返回值1为质数 0不是质数
    // 除法运算是对称的 开根号的目的减少循环的次数
    // 但是又能达到变量否能整除 所需测试的数字
    // 以100 为例 只需要判断2 到 10,  判断了 2 就判断了 50, 10是一个分界点
    for (int i = 2; i <= sqrt(num); i++)
    {
        if(num % i == 0) {
            return 0;
        }
    }

    return 1;
    
}

int main(int argc, char const *argv[])
{
    printf("%d \n", isPrime(11));
    return 0;
}
