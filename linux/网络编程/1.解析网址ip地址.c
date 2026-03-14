#include "myhead.h"

int main(int argc, char const *argv[])
{
    if(argc < 2)
    {
        printf("忘记传网址了！\n");
        return -1;
    }

    struct hostent *result = gethostbyname(argv[1]);
    if(result == NULL)
    {
        perror("解析域名失败了！\n");
        return -1;
    }

    printf("网站官方名字是：%s\n",result->h_name);
    printf("网站别名是：%s\n",result->h_aliases[0]);
    printf("网站官方ip地址是：%s\n",inet_ntoa(*((struct in_addr *)(result->h_addr_list[0]))));



    return 0;
}
