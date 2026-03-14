#include "myhead.h"

cart_item_t shopping_cart[MAX_CART_ITEMS] = {0};
int cart_item_count = 0  ;
// 分离的商品列表：
// 1. purchase_goods_list: 购物页面的商品列表（只读，管理员操作不影响）
// 2. admin_goods_list: 管理员页面的商品列表（可编辑）
goods_item_t purchase_goods_list[MAX_GOODS_COUNT] = {0}; // 购物页面商品列表
int purchase_goods_count                          = 0;   // 购物页面商品数量

goods_item_t admin_goods_list[MAX_GOODS_COUNT] = {0}; // 管理员页面商品列表
int admin_goods_count                          = 0;   // 管理员页面商品数量


int main(void)
{
    lv_init();

    fbdev_init();
    static lv_color_t buf[DISP_BUF_SIZE];
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = fbdev_flush;
    disp_drv.hor_res  = 800;
    disp_drv.ver_res  = 480;
    lv_disp_drv_register(&disp_drv);

    evdev_init();
    static lv_indev_drv_t indev_drv_1;
    lv_indev_drv_init(&indev_drv_1);
    indev_drv_1.type    = LV_INDEV_TYPE_POINTER;
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
