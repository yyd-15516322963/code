#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define DISP_BUF_SIZE (800*480)

// void bt1_cb(lv_event_t * e)
// {
//     printf("登录成功！\n");
//     printf("主函数传递的信息是：%d\n",*((int *)(e->user_data)));
// }

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
    lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);

    /*
        自己手写代码创建按钮
           1.LVGL函数命名很有规律
             创建某个组件函数名一定叫做 lv_xxx_create()  xxx就是组件的英文单词
             组件/控件:做GUI界面需要用到各种小部件(按钮,文本输入框,标签..),都叫做组件
             lv_obj_set_xxx();
           2.lv_obj_t是结构体(通用类型)用来表示LVGL中的任意一个组件
           3.父窗口:你的组件在哪个窗口上显示,这个窗口就是父窗口
             lv_btn_create(当前窗口的地址);
             当前窗口的地址: lv_scr_act()返回的是当前活跃屏幕的地址
           4.按钮默认在左上角显示,没有文字,没有背景图片,大小不合适  
               
    */
    //创建按钮
    lv_obj_t *bt1=lv_btn_create(lv_scr_act());

    //设置按钮的坐标位置和大小-->设置某个属性
    lv_obj_set_size(bt1,200,200); 
    lv_obj_set_pos(bt1,300,100);

    //给按钮设置文字-->LVGL没有提供专门的方法
    //思路:创建一个标签(专门显示文字的),把标签嵌套到按钮上就可以显示文字

    lv_obj_t *img1 =lv_img_create(bt1); 

    //给标签设置文字内容
    // lv_label_set_text(lb1,"login");
    lv_img_set_src(img1, "S:/zhou.jpg");

    //lv_label_set_text(lb1,"登录");  //目前没有移植中文字库,无法使用中文

    // int n1 = 666;
    // lv_obj_add_event_cb(bt1, bt1_cb, LV_EVENT_CLICKED, &n1);

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
