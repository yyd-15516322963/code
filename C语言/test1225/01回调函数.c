#include <stdio.h>
#include <unistd.h>

// 模拟网络请求
void get_data_from_http_server(char *url, void(*callback)(void))
{
    printf("开始请求 %s 的网络数据\n", url);
    sleep(2);
    printf(" %s 的数据请求成功\n", url);
    callback();
}


void play_live()
{
    printf("开始直播了 \n");
}

void play_danmu()
{
    printf("弹幕开始滚动 \n");
}
int main(int argc, char const *argv[])
{
    get_data_from_http_server("www.bilibili-live.com", play_live);


    get_data_from_http_server("www.bilibili-danmu.com", play_danmu);
    return 0;
}
