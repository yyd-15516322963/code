// 输入某年某月某日，判断这一天是这一年的第几天？
#include <stdio.h>
int main()
{
    int year, mouth, day;
    int DayMouth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    scanf("%d %d %d", &year, &mouth, &day);
    int sum = 0;

    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
    {
        DayMouth[1] = 29;
    }

    for (int i = 0; i < mouth-1; i++)
    {
        sum += DayMouth[i];
    }
    sum += day;
    printf("%d\n", sum);
}