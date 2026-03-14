#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>


#define DISP_BUF_SIZE (800*480)

//四个按钮
lv_obj_t *bt1;
lv_obj_t *bt2;
lv_obj_t *bt3;
lv_obj_t *bt4;
//定时器
lv_timer_t *timer1=NULL;
//自己写个函数:实现初始化字体跟样式,返回初始化好的样式
/*
    参数:fontpath 字体路径名
         fontsize  字体大小
*/
lv_style_t *my_lv_create_style(const char *fontpath,int fontsize)
{
    static lv_ft_info_t info;  //lv_ft_info_t是结构体类型,用来存放字体的信息
    info.name = fontpath; //开发板上中文字体的路径名
    info.weight = fontsize;  //字体大小
    info.style = FT_FONT_STYLE_NORMAL;
    info.mem = NULL;
    if(!lv_ft_font_init(&info)) {  //初始化字体
        LV_LOG_ERROR("create failed\n");  //开发板打印字符要加上\n
    }

    //定义样式对象
    lv_style_t *style=malloc(sizeof(lv_style_t));
    //初始化样式
    lv_style_init(style);
    //样式使用刚才的字体
    lv_style_set_text_font(style, info.font);
    //样式设置文字对齐方式-->居中对齐(靠左,靠右,居中)
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    return style;
}

//定时器的事件响应函数
void timer_cb(lv_timer_t* timer)
{
    printf("你好粤嵌!\n");
}

//四个按钮共用同一个事件响应函数
void bt_cb(lv_event_t *e)
{
    //判断点击的是哪个按钮
    if(e->target==bt2)
        lv_timer_enable(false); //关闭定时器
    else if(e->target==bt3)
        lv_timer_pause(timer1); //暂停定时器
    else if(e->target==bt4)
        lv_timer_resume(timer1); //恢复/继续定时器
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
        定时器的使用
          1.如果你代码重复不断的做某个事情-->用定时器
            用定时器代替死循环
    */
    //创建定时器
    timer1=lv_timer_create(timer_cb,1000,NULL); //每隔1000ms执行一次timer_cb

    //创建四个按钮
    bt1=lv_btn_create(lv_scr_act());
    bt2=lv_btn_create(lv_scr_act());
    bt3=lv_btn_create(lv_scr_act());
    bt4=lv_btn_create(lv_scr_act());

    //设置按钮的坐标位置和大小-->设置某个属性
    lv_obj_set_size(bt1,100,100); 
    lv_obj_set_pos(bt1,400,100);
    lv_obj_set_size(bt2,100,100); 
    lv_obj_set_pos(bt2,600,100);
    lv_obj_set_size(bt3,100,100); 
    lv_obj_set_pos(bt3,400,300);
    lv_obj_set_size(bt4,100,100); 
    lv_obj_set_pos(bt4,600,300);

    //创建标签
    lv_obj_t *lb1=lv_label_create(bt1);  //按钮作为标签的父窗口
    lv_obj_t *lb2=lv_label_create(bt2);
    lv_obj_t *lb3=lv_label_create(bt3);
    lv_obj_t *lb4=lv_label_create(bt4);
    //准备好一个样式
    lv_style_t *style1=my_lv_create_style("/font/simkai.ttf",36);
    //标签使用刚才的样式-->一定要写,不写前功尽弃
    lv_obj_add_style(lb1, style1, 0);
    lv_label_set_text(lb1,"开启");  
    lv_obj_add_style(lb2, style1, 0);
    lv_label_set_text(lb2,"关闭"); 
    lv_obj_add_style(lb3, style1, 0);
    lv_label_set_text(lb3,"暂停"); 
    lv_obj_add_style(lb4, style1, 0);
    lv_label_set_text(lb4,"恢复"); 

    //四个按钮添加事件响应函数
    lv_obj_add_event_cb(bt1,bt_cb,LV_EVENT_CLICKED,NULL);
    lv_obj_add_event_cb(bt2,bt_cb,LV_EVENT_CLICKED,NULL);
    lv_obj_add_event_cb(bt3,bt_cb,LV_EVENT_CLICKED,NULL);
    lv_obj_add_event_cb(bt4,bt_cb,LV_EVENT_CLICKED,NULL);

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
