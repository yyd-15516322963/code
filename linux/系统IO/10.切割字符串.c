#include <string.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    char time[20] = "2026-1-19";

    // char *p;
    // p = strtok(time, "- :");
    // printf("%s\n", p);
    // while((p = strtok(NULL,"- :")) != NULL)
    // {
    //     printf("%s\n", p);
    // }

    // 字符串切不了
    char year[10], month[10], day[10];
    sscanf(time, "%s-%s-%s", year, month, day);
    printf("%s %s %s\n", year, month, day);

    return 0;
}
