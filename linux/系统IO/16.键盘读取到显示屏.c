#include "myhead.h"

int main(int argc, char const *argv[])
{

    char buf[10];
    
        printf("请输入字符串：\n");
        read(STDIN_FILENO, buf, 10);

        write(STDOUT_FILENO, buf, 10);

    return 0;
}
