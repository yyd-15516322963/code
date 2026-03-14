#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>  // 新增：用于strcmp的头文件
#include "myhead.h"

#define DISP_BUF_SIZE (800*480)

lv_obj_t *page1;
lv_obj_t *page2;
lv_obj_t *bt_key;
lv_obj_t *bt1;
lv_obj_t *bt2;
lv_obj_t *kb;
lv_obj_t *ta1;
lv_obj_t *ta2;
const char *name;
const char *pwd;

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



// ========== 新增/修改：文本框聚焦事件（弹出键盘） ==========
void ta1_cb(lv_event_t *e)
{
    lv_keyboard_set_textarea(kb,ta1);
    // LVGL 8.3：清除隐藏标志，显示键盘
    lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    // 强制刷新，确保显示
    lv_refr_now(lv_disp_get_default());
}

void ta2_cb(lv_event_t *e)
{
    lv_keyboard_set_textarea(kb,ta2);
    lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_refr_now(lv_disp_get_default());
}


const char *btns[]={"continue","close",""};  //给消息盒子两个按钮设置字面值

// 消息盒子
void bt_msgbox_cb(lv_event_t *e)
{
    //弹出消息盒子
    lv_obj_t *msgbox = lv_msgbox_create(page1, "Warning", "The password/account you entered is incorrect.Please click the 'reg' button to complete the registration process.", btns, true ); 
    lv_obj_set_pos(msgbox, 260, 80);

}

// 登录按钮回调函数
void bt_login_cb(lv_event_t * e)
{
    // 修复：添加const避免警告，空指针保护
    const char *name = lv_textarea_get_text(ta1);
    const char *pwd = lv_textarea_get_text(ta2);
    
    // 空指针保护，避免strcmp崩溃
    if(name == NULL) name = "";
    if(pwd == NULL) pwd = "";

    if(strcmp(name,"yyd")==0 && strcmp(pwd,"123456")==0)
    {
        
        //跳到主界面
        show_login_page2();
        //关闭释放登录界面
        lv_obj_del(page1);
    }
    else
    {
        lv_obj_add_event_cb(bt1,bt_msgbox_cb,LV_EVENT_CLICKED,NULL);
    }
}

// 注册按钮回调函数
void bt_regt_cb()
{
    char buf[100];
    sprintf(buf, "%s@%s",name, pwd);
    printf("%s\n", buf);
    my_write(buf);
}
void bt_key_cb()
{
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
}

void show_login_page1()
{
    page1 = lv_obj_create(lv_scr_act()); 
    lv_obj_set_size(page1, 800, 800);

    // 退出键盘按钮
    bt_key = lv_btn_create(page1);
    lv_obj_t *lb_key = lv_label_create(bt_key);
    lv_label_set_text(lb_key, "quit");
    lv_obj_set_size(bt_key, 60, 30);
    lv_obj_set_pos(bt_key, 0, 200);
    lv_obj_add_event_cb(bt_key, bt_key_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(bt_key, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bt_key, 255, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lb_key, lv_color_hex(0x000000), LV_STATE_DEFAULT);


    // 创建登录退出按钮
    bt1 = lv_btn_create(page1);
    bt2 = lv_btn_create(page1);
    lv_obj_set_size(bt1, 60, 30);
    lv_obj_set_pos(bt1, 300, 280);
    lv_obj_set_size(bt2, 60, 30);
    lv_obj_align_to(bt2,bt1, LV_ALIGN_OUT_RIGHT_MID,70,0);
    lv_obj_set_style_bg_color(bt1, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bt1, 255, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(bt2, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bt2, 255, LV_STATE_DEFAULT);

    
    

    // 按钮写入内容
    lv_obj_t *lb1 = lv_label_create(bt1);
    lv_label_set_text(lb1, "login");
    lv_obj_t *lb2 = lv_label_create(bt2);
    lv_label_set_text(lb2, "regt");
    lv_obj_set_style_text_color(lb1, lv_color_hex(0x000000), LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lb2, lv_color_hex(0x000000), LV_STATE_DEFAULT);

    // 创建标题标签
    lv_obj_t *lb_title = lv_label_create(page1);
    lv_label_set_text(lb_title, "Welcome Back");
     lv_obj_align(lb_title, LV_ALIGN_CENTER,0,-370);
    lv_obj_set_style_text_font(lb_title, &lv_font_montserrat_36, LV_STATE_DEFAULT);
    lv_obj_t *lb_title1 = lv_label_create(page1);
    lv_label_set_text(lb_title1, "------------------LOGIN WITH EMAIL------------------");
    lv_obj_align_to(lb_title1,lb_title, LV_ALIGN_OUT_BOTTOM_MID,0,0);
    lv_obj_set_style_text_font(lb_title1, &lv_font_montserrat_12, LV_STATE_DEFAULT);

    // 创建账户密码标签，写入内容
    lv_obj_t *lb3 = lv_label_create(page1);
    lv_label_set_text(lb3, "Address");
    lv_obj_t *lb4 = lv_label_create(page1);
    lv_label_set_text(lb4, "password");
    lv_obj_set_pos(lb3, 250, 90);
    lv_obj_set_pos(lb4, 250, 170);


    // 创建文本区域
    ta1 = lv_textarea_create(page1);
    ta2 = lv_textarea_create(page1);
    lv_obj_set_size(ta1, 300, 50);
    lv_obj_set_pos(ta1, 250, 110);
    lv_obj_set_size(ta2, 300, 50);
    lv_obj_set_pos(ta2, 250, 190);

    //设置文本区域只能显示一行(不会自动换行)
    lv_textarea_set_one_line(ta1, true);
    lv_textarea_set_one_line(ta2, true);

    // ========== 修改：创建软键盘（默认隐藏+调整位置） ==========
    kb = lv_keyboard_create(lv_scr_act());
    lv_obj_set_size(kb,800,200);
    // LVGL 8.3：调整键盘到屏幕底部（避免遮挡输入框）
    lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
    // 默认隐藏键盘
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_event_cb(ta1,ta1_cb,LV_EVENT_FOCUSED,NULL);
    lv_obj_add_event_cb(ta2,ta2_cb,LV_EVENT_FOCUSED,NULL);

    lv_obj_add_event_cb(bt1, bt_login_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(bt2, bt_regt_cb, LV_EVENT_CLICKED, NULL);


}

void bt2_cb(lv_event_t * e)
{
    
}
void bt_q_cb(lv_event_t * e)
{
    show_login_page1();
    lv_obj_del(page2);
}

void show_login_page2()
{
    page2 = lv_obj_create(lv_scr_act()); 
    lv_obj_set_size(page2, 800, 480);
    
    lv_obj_t *tabview = lv_tabview_create(page2, LV_DIR_BOTTOM, 50);
    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Home Page");
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Purchase");
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "Shopping cart");
    lv_obj_t *tab4 = lv_tabview_add_tab(tabview, "Personal Center");

    lv_tabview_set_act(tabview, 0, LV_ANIM_OFF);

    //创建商品标签
    lv_obj_t *lb_goods1 = lv_btn_create(tab1);
    lv_obj_set_size(lb_goods1,100,200); 
    lv_obj_set_pos(lb_goods1,50,100);
    // lv_obj_t *lb_goods2 = lv_label_create(tab1);
    // lv_obj_t *lb_goods3 = lv_label_create(tab1);
    // lv_obj_t *lb_goods4 = lv_label_create(tab1);
    lv_obj_t *img1 =lv_img_create(lb_goods1); 
    lv_img_set_src(img1, "S:/zhou.jpg");


    lv_obj_t *bt_q = lv_btn_create(tab1);
    lv_obj_t *lb_q = lv_label_create(bt_q);
    lv_label_set_text(lb_q, "quit");
    lv_obj_set_size(bt_q, 60, 30);
    lv_obj_set_pos(bt_q, 650, 0);
    lv_obj_set_style_bg_color(bt_q, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bt_q, 255, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lb_q, lv_color_hex(0x000000), LV_STATE_DEFAULT);

    lv_obj_add_event_cb(bt_q, bt_q_cb, LV_EVENT_CLICKED, NULL);
}

int main(void)
{
    lv_init();

    fbdev_init();
    static lv_color_t buf[DISP_BUF_SIZE];
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = 800;
    disp_drv.ver_res    = 480;
    lv_disp_drv_register(&disp_drv);

    evdev_init();
    static lv_indev_drv_t indev_drv_1;
    lv_indev_drv_init(&indev_drv_1);
    indev_drv_1.type = LV_INDEV_TYPE_POINTER;
    indev_drv_1.read_cb = evdev_read;
    lv_indev_drv_register(&indev_drv_1);

    show_login_page1();

    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}

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