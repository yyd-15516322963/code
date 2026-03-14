#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>

#define DISP_BUF_SIZE (800*480)

lv_obj_t *win1;  //小窗口1
lv_obj_t *win2;  //小窗口2

lv_obj_t *checkbox1;  //复选框1
lv_obj_t *checkbox2;  //复选框2

// 复选框对应的事件响应函数
void checkbox1_cb(lv_event_t *e)
{
    //获取复选框的状态-->通过状态值判定复选框是开启还是关闭
    if(lv_obj_has_state(checkbox1, LV_STATE_CHECKED))
        printf("复选框1 打开了\n");
    else
        printf("复选框1 关闭了\n");
}
void checkbox2_cb(lv_event_t *e)
{
    //获取复选框的状态-->通过状态值判定复选框是开启还是关闭
    if(lv_obj_has_state(checkbox2, LV_STATE_CHECKED))
        printf("复选框2 打开了\n");
    else
        printf("复选框2 关闭了\n");
}

int main(void)
{
    //LVGL的初始化,一定不能删除,做了核心功能的初始化
    lv_init();

    //液晶屏的初始化-->open(打开液晶屏)-->ioctl(获取液晶屏参数)-->mmap(映射得到首地址)
    fbdev_init();

    //定义数组存放要显示的内容
    static lv_color_t buf[DISP_BUF_SIZE];

    //初始化数组
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    //结构体变量,成员特别多
    static lv_disp_drv_t disp_drv;
    //初始化刚才定义的结构体变量
    lv_disp_drv_init(&disp_drv);
    //对结构体里面的成员赋值
    disp_drv.draw_buf   = &disp_buf;    //你刚才定义初始化的缓冲区
    disp_drv.flush_cb   = fbdev_flush;  //函数指针,很重要,用来在液晶屏上画点/画矩形
    disp_drv.hor_res    = 800;  //设置液晶屏屏幕的宽
    disp_drv.ver_res    = 480;  //设置液晶屏屏幕的高
    lv_disp_drv_register(&disp_drv);

    //触摸屏初始化
    evdev_init();
    //定义结构体存放了触摸屏的信息
    static lv_indev_drv_t indev_drv_1;
    //初始化结构体
    lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
    indev_drv_1.type = LV_INDEV_TYPE_POINTER;

    //很重要: 触摸屏操作的核心函数
    indev_drv_1.read_cb = evdev_read; //读取保存触摸屏坐标
    lv_indev_drv_register(&indev_drv_1);

     /*
        自己手写代码创建小窗口-->嵌套到当前活跃屏幕
         
               
    */
    //创建小窗口
    win1=lv_obj_create(lv_scr_act());
    win2=lv_obj_create(lv_scr_act());

    //设置小窗口的坐标位置和大小-->设置某个属性
    lv_obj_set_size(win1,300,230); 
    lv_obj_set_pos(win1,490,0);
    lv_obj_set_size(win2,300,230); 
    lv_obj_set_pos(win2,490,240);

    //往小窗口里面添加你需要的组件(如果不添加,两个小窗口都是空白)
    checkbox1 = lv_checkbox_create(win1);
    checkbox2 = lv_checkbox_create(win1);

    lv_obj_set_size(checkbox1, 100,50);
    lv_obj_set_pos(checkbox1, 10,10);
    lv_obj_set_size(checkbox2, 100,50);
    lv_obj_set_pos(checkbox2, 10,60);

    //给复选框设置事件响应函数
    lv_obj_add_event_cb(checkbox1,checkbox1_cb,LV_EVENT_VALUE_CHANGED,NULL);
    lv_obj_add_event_cb(checkbox2,checkbox2_cb,LV_EVENT_VALUE_CHANGED,NULL);

    lv_checkbox_set_text_static(checkbox1, "open");
    lv_checkbox_set_text_static(checkbox2, "close");



    //防止主程序退出,lv_timer_handler();用来进行心跳检测
    while(1) {
        lv_timer_handler();  //心跳函数,用来检测输入设备事件/刷新界面/...
        usleep(5000);
    }

    return 0;
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}