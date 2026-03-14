#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>


#define DISP_BUF_SIZE (800*480)

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

lv_obj_t *ta1;
lv_obj_t *ta2;
lv_obj_t *kb;
void ta1_cb(lv_event_t *e)
{
    lv_keyboard_set_textarea(kb,ta1);
}
void ta2_cb(lv_event_t *e)
{
    lv_keyboard_set_textarea(kb,ta2);
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

    lv_obj_t *tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);
    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "tab1");
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "tab2");
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "tab3");
    lv_obj_t *tab4 = lv_tabview_add_tab(tabview, "tab4");

    lv_tabview_set_act(tabview, 0, LV_ANIM_ON);

    // 创建登录退出按钮
    lv_obj_t *bt1 = lv_btn_create(tab1);
    lv_obj_t *bt2 = lv_btn_create(tab1);
    lv_obj_set_size(bt1, 70, 40);
    lv_obj_set_pos(bt1, 220, 160);
    lv_obj_set_size(bt2, 70, 40);
    lv_obj_set_pos(bt2, 320, 160);

    
    // 按钮写入内容
    lv_obj_t *lb1 = lv_label_create(bt1);
    lv_label_set_text(lb1, "login");
    lv_obj_t *lb2 = lv_label_create(bt2);
    lv_label_set_text(lb2, "quit");


    // 创建账户密码标签，写入内容
    lv_obj_t *lb3 = lv_label_create(tab1);
    lv_label_set_text(lb3, "acnt");
    lv_obj_t *lb4 = lv_label_create(tab1);
    lv_label_set_text(lb4, "pwd");
    // lv_obj_set_size(lb3, 60, 40);
    lv_obj_set_pos(lb3, 150, 40);
    // lv_obj_set_size(lb4, 60, 40);
    lv_obj_set_pos(lb4, 150, 90);


    // 创建文本区域
    ta1 = lv_textarea_create(tab1);
    ta2 = lv_textarea_create(tab1);
    lv_obj_set_size(ta1, 200, 40);
    lv_obj_set_pos(ta1, 200, 30);
    lv_obj_set_size(ta2, 200, 40);
    lv_obj_set_pos(ta2, 200, 80);

    //设置文本区域只能显示一行(不会自动换行)
    lv_textarea_set_one_line(ta1, true);
    lv_textarea_set_one_line(ta2, true);

    // 创建软键盘
    kb = lv_keyboard_create(lv_scr_act());

    lv_obj_set_size(kb,800,200);
    //注意注意: 软键盘比较特殊,如果不是0,0会偏移很多
    lv_obj_set_pos(kb,0,0);

    lv_obj_add_event_cb(ta1,ta1_cb,LV_EVENT_FOCUSED,NULL);
    lv_obj_add_event_cb(ta2,ta2_cb,LV_EVENT_FOCUSED,NULL);

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
