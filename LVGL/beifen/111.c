#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define DISP_BUF_SIZE (800 * 480)

// 全局UI对象
lv_obj_t * page1;
lv_obj_t * page2;
lv_obj_t * sec_page1;
lv_obj_t * sec_page2;
lv_obj_t * bt_key;
lv_obj_t * bt_key2;
lv_obj_t * bt1;
lv_obj_t * bt2;
lv_obj_t * kb;
lv_obj_t * ta1;
lv_obj_t * ta2;
lv_obj_t * tab1;
lv_obj_t * tab2;
lv_obj_t * tab3; // Shopping Car页面
lv_obj_t * tab4;
lv_obj_t * tab2_1;
lv_obj_t * tab2_2;
lv_obj_t * tab2_3;
lv_obj_t * sec_bt1;
lv_obj_t * sec_bt2;
lv_obj_t * sec_ta1;
lv_obj_t * sec_ta2;
lv_obj_t * sec_kb1;
lv_obj_t * sec_kb2;
lv_obj_t * pay_ui;

// 管理员操作表单控件
lv_obj_t * admin_ta_id;            // 商品ID输入框
lv_obj_t * admin_ta_name;          // 商品名称输入框
lv_obj_t * admin_ta_price;         // 商品价格输入框
lv_obj_t * admin_ta_img;           // 图片路径输入框
lv_obj_t * admin_goods_list_label; // 商品列表展示标签

// ========== 商品数据结构重构（动态化） ==========
// 商品结构体
typedef struct
{
    int id;             // 商品唯一ID
    char name[1000];    // 商品名称（固定内存）
    char price[32];     // 商品价格（固定内存）
    char img_path[128]; // 商品图片路径（固定内存）
} goods_item_t;

// 购物车项（商品+数量）
typedef struct
{
    goods_item_t goods;
    int count;
} cart_item_t;

// 全局购物车（最大20个商品）
#define MAX_CART_ITEMS 20
cart_item_t shopping_cart[MAX_CART_ITEMS] = {0};
int cart_item_count                       = 0;

// 动态商品列表配置
#define MAX_GOODS_COUNT 50

// 分离的商品列表：
// 1. purchase_goods_list: 购物页面的商品列表（只读，管理员操作不影响）
// 2. admin_goods_list: 管理员页面的商品列表（可编辑）
goods_item_t purchase_goods_list[MAX_GOODS_COUNT] = {0}; // 购物页面商品列表
int purchase_goods_count                          = 0;   // 购物页面商品数量

goods_item_t admin_goods_list[MAX_GOODS_COUNT] = {0}; // 管理员页面商品列表
int admin_goods_count                          = 0;   // 管理员页面商品数量

// ========== 定时器回调函数 ==========
void toast_timer_cb(lv_timer_t * timer)
{
    lv_obj_t * toast = (lv_obj_t *)timer->user_data;
    if(toast && lv_obj_is_valid(toast)) {
        lv_obj_del(toast);
    }
    lv_timer_del(timer);
}

// ========== 购物页面商品数据层核心函数（只读） ==========
// 初始化默认商品（购物页面）
void init_default_purchase_goods()
{
    purchase_goods_count = 0;

    goods_item_t g1 = {0};
    g1.id           = 1001;
    strcpy(g1.name, "HKC 27-inch 2K 300Hz gaming monitor G27H4Plus");
    strcpy(g1.price, "$ 1199");
    strcpy(g1.img_path, "S:/goods1.png");
    purchase_goods_list[purchase_goods_count++] = g1;

    goods_item_t g2 = {0};
    g2.id           = 1002;
    strcpy(g2.name, "Titan Legion 27-inch 4K 160Hz gaming monitor P2710V");
    strcpy(g2.price, "$ 1099");
    strcpy(g2.img_path, "S:/goods2.png");
    purchase_goods_list[purchase_goods_count++] = g2;

    goods_item_t g3 = {0};
    g3.id           = 1003;
    strcpy(g3.name, "Samsung 31.5-inch 4K Display U32J590UQC");
    strcpy(g3.price, "$ 2099");
    strcpy(g3.img_path, "S:/goods3.png");
    purchase_goods_list[purchase_goods_count++] = g3;

    goods_item_t g4 = {0};
    g4.id           = 1004;
    strcpy(g4.name, "MSI Black Edge X30 27-inch 2K 300Hz Gaming Monitor");
    strcpy(g4.price, "$ 1299");
    strcpy(g4.img_path, "S:/goods4.png");
    purchase_goods_list[purchase_goods_count++] = g4;

    goods_item_t g5 = {0};
    g5.id           = 1006;
    strcpy(g5.name, "MSI Intel Core 14600KF Graphics Card Computer");
    strcpy(g5.price, "$ 3099");
    strcpy(g5.img_path, "S:/zj1.png");
    purchase_goods_list[purchase_goods_count++] = g5;

    goods_item_t g6 = {0};
    g6.id           = 1007;
    strcpy(g6.name, "ASUS ROG Nightstalker Complete System 3");
    strcpy(g6.price, "$ 13499");
    strcpy(g6.img_path, "S:/zj2.png");
    purchase_goods_list[purchase_goods_count++] = g6;

    goods_item_t g7 = {0};
    g7.id           = 1008;
    strcpy(g7.name, "ASUS ROG Nightstalker Complete System 3");
    strcpy(g7.price, "$ 13499");
    strcpy(g7.img_path, "S:/zj3.png");
    purchase_goods_list[purchase_goods_count++] = g7;

    goods_item_t g8 = {0};
    g8.id           = 1009;
    strcpy(g8.name, "ASUS ROG Nightstalker Complete System 3");
    strcpy(g8.price, "$ 13499");
    strcpy(g8.img_path, "S:/zj4.png");
    purchase_goods_list[purchase_goods_count++] = g8;
}

// 初始化管理员页面商品列表（可独立初始化）
void init_default_admin_goods()
{
    admin_goods_count = 0;
    // 管理员页面初始为空列表，或复制购物页面商品
    for(int i = 0; i < purchase_goods_count; i++) {
        admin_goods_list[admin_goods_count] = purchase_goods_list[i];
        admin_goods_count++;
    }
}

// 从购物页面商品列表查询商品（购物车使用）
goods_item_t * query_purchase_goods(int goods_id)
{
    for(int i = 0; i < purchase_goods_count; i++) {
        if(purchase_goods_list[i].id == goods_id) {
            return &purchase_goods_list[i];
        }
    }
    return NULL;
}

// ========== 管理员页面商品数据层核心函数 ==========
// 1. 新增商品到管理员列表（0成功/-1满/-2ID重复）
int admin_add_goods(goods_item_t new_goods)
{
    if(admin_goods_count >= MAX_GOODS_COUNT) return -1;
    // 检查ID重复
    for(int i = 0; i < admin_goods_count; i++) {
        if(admin_goods_list[i].id == new_goods.id) return -2;
    }
    goods_item_t * goods = &admin_goods_list[admin_goods_count];
    goods->id            = new_goods.id;

    // 拷贝名称（带默认值，防止空指针）
    if(new_goods.name[0] != '\0') { // 检查非空
        strncpy(goods->name, new_goods.name, sizeof(goods->name) - 1);
        goods->name[sizeof(goods->name) - 1] = '\0'; // 确保终止符
    } else {
        strcpy(goods->name, "Unnamed Goods");
    }

    // 拷贝价格
    if(new_goods.price[0] != '\0') {
        strncpy(goods->price, new_goods.price, sizeof(goods->price) - 1);
        goods->price[sizeof(goods->price) - 1] = '\0';
    } else {
        strcpy(goods->price, "$ 0");
    }

    // 拷贝图片路径
    if(new_goods.img_path[0] != '\0') {
        strncpy(goods->img_path, new_goods.img_path, sizeof(goods->img_path) - 1);
        goods->img_path[sizeof(goods->img_path) - 1] = '\0';
    } else {
        strcpy(goods->img_path, "S:/default.png");
    }

    admin_goods_count++;
    return 0;
}

// 2. 从管理员列表删除商品
int admin_delete_goods(int goods_id)
{
    int found = -1;
    for(int i = 0; i < admin_goods_count; i++) {
        if(admin_goods_list[i].id == goods_id) {
            found = i;
            break;
        }
    }
    if(found == -1) return -1;

    // 数组前移覆盖
    for(int i = found; i < admin_goods_count - 1; i++) {
        admin_goods_list[i] = admin_goods_list[i + 1];
    }
    memset(&admin_goods_list[admin_goods_count - 1], 0, sizeof(goods_item_t));
    admin_goods_count--;
    return 0;
}

// 3. 修改管理员列表中的商品
int admin_update_goods(int goods_id, goods_item_t new_info)
{
    for(int i = 0; i < admin_goods_count; i++) {
        if(admin_goods_list[i].id == goods_id) {
            // 名称更新
            if(new_info.name[0] != '\0') {
                strncpy(admin_goods_list[i].name, new_info.name, sizeof(admin_goods_list[i].name) - 1);
                admin_goods_list[i].name[sizeof(admin_goods_list[i].name) - 1] = '\0';
            }
            // 价格更新
            if(new_info.price[0] != '\0') {
                strncpy(admin_goods_list[i].price, new_info.price, sizeof(admin_goods_list[i].price) - 1);
                admin_goods_list[i].price[sizeof(admin_goods_list[i].price) - 1] = '\0';
            }
            // 图片路径更新
            if(new_info.img_path[0] != '\0') {
                strncpy(admin_goods_list[i].img_path, new_info.img_path, sizeof(admin_goods_list[i].img_path) - 1);
                admin_goods_list[i].img_path[sizeof(admin_goods_list[i].img_path) - 1] = '\0';
            }
            return 0;
        }
    }
    return -1;
}

// 4. 从管理员列表查询商品
goods_item_t * admin_query_goods(int goods_id)
{
    for(int i = 0; i < admin_goods_count; i++) {
        if(admin_goods_list[i].id == goods_id) {
            return &admin_goods_list[i];
        }
    }
    return NULL;
}

// ========== 购物车相关函数 ==========
// 添加商品到购物车（从购物页面列表）
int add_goods_to_cart(const goods_item_t * goods)
{
    if(goods == NULL) return -1;

    // 检查商品是否已存在
    for(int i = 0; i < cart_item_count; i++) {
        if(shopping_cart[i].goods.id == goods->id) {
            shopping_cart[i].count++;
            return 1; // 数量+1
        }
    }

    // 检查购物车是否已满
    if(cart_item_count >= MAX_CART_ITEMS) {
        return -1; // 购物车满
    }

    // 添加新商品
    shopping_cart[cart_item_count].goods = *goods;
    shopping_cart[cart_item_count].count = 1;
    cart_item_count++;
    return 0; // 添加成功
}

// 清空购物车回调
void btn_clear_car_cb(lv_event_t * e)
{
    cart_item_count = 0;
    memset(shopping_cart, 0, sizeof(shopping_cart));

    // 空指针检查
    if(tab3 && lv_obj_is_valid(tab3)) {
        lv_obj_clean(tab3);

        // 空购物车提示
        lv_obj_t * empty_label = lv_label_create(tab3);
        lv_label_set_text(empty_label, "Your shopping cart is empty!\nAdd some goods from Purchase page.");
        lv_obj_set_style_text_align(empty_label, LV_TEXT_ALIGN_CENTER, LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(empty_label, &lv_font_montserrat_18, LV_STATE_DEFAULT);
        lv_obj_center(empty_label);
    }
    // 提示框
    lv_obj_t * toast = lv_label_create(lv_scr_act());
    lv_label_set_text(toast, "Cart cleared successfully!");
    lv_obj_set_style_text_color(toast, lv_color_black(), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(toast, &lv_font_montserrat_16, LV_STATE_DEFAULT);
    lv_obj_align(toast, LV_ALIGN_TOP_MID, 20, 20);

    lv_timer_t * timer = lv_timer_create(toast_timer_cb, 2000, toast);

    refresh_shopping_cart_page();
}

// 支付UI对象
lv_obj_t * pay_ui = NULL;

// 支付消息框回调
void pay_btn_msgbox_cb()
{
    if(tab3 && lv_obj_is_valid(tab3)) {
        const char * btns[]   = {"", "", ""};
        lv_obj_t * pay_msgbox = lv_msgbox_create(
            tab3, "Warning", "Your shopping cart is empty! \nPayment failed. Please add the item and try again.", btns,
            true);
        lv_obj_set_pos(pay_msgbox, 260, 80);
    }
}

// 退出支付回调
void btn_pay_q_cb(lv_event_t * e)
{
    if(pay_ui && lv_obj_is_valid(pay_ui)) {
        lv_obj_del(pay_ui);
        pay_ui = NULL;
    }

    // 返回主页面
    show_login_page2();

    refresh_shopping_cart_page();
}

// 完成支付回调
void btn_pay_fin_cb(lv_event_t * e)
{
    if(pay_ui && lv_obj_is_valid(pay_ui)) {
        lv_obj_del(pay_ui);
        pay_ui = NULL;
    }

    // 清空购物车
    cart_item_count = 0;
    memset(shopping_cart, 0, sizeof(shopping_cart));

    // 返回主页面
    show_login_page2();

    // 显示支付成功提示
    lv_obj_t * toast = lv_label_create(lv_scr_act());
    lv_label_set_text(toast, "Payment successful! Cart cleared.");
    lv_obj_set_style_text_color(toast, lv_color_black(), LV_STATE_DEFAULT);
    lv_obj_align(toast, LV_ALIGN_TOP_MID, 20, 20);
    lv_obj_set_style_text_font(toast, &lv_font_montserrat_16, LV_STATE_DEFAULT);

    lv_timer_t * timer = lv_timer_create(toast_timer_cb, 2000, toast);

    refresh_shopping_cart_page();
}

// 支付按钮回调
void pay_car_cb(lv_event_t * e)
{

    lv_obj_set_style_pad_all(tab3, 0, 0);
    lv_obj_set_style_border_width(tab3, 0, 0);

    // 检查购物车是否为空
    if(cart_item_count == 0) {
        pay_btn_msgbox_cb();
        return;
    }

    // 清空购物车页面
    if(tab3 && lv_obj_is_valid(tab3)) {
        lv_obj_clean(tab3);

        // 创建支付UI容器
        pay_ui = lv_obj_create(tab3);
        lv_obj_set_size(pay_ui, 800, 480);
        lv_obj_set_style_pad_all(pay_ui, 0, 0);
        lv_obj_set_style_border_width(pay_ui, 0, 0);

        // 支付二维码图片
        lv_obj_t * pay_btn = lv_btn_create(pay_ui);
        lv_obj_set_size(pay_btn, 293, 400);
        lv_obj_set_pos(pay_btn, 400, 0);
        lv_obj_set_style_radius(pay_btn, 8, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(pay_btn, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(pay_btn, 1, LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(pay_btn, lv_color_hex(0xCCCCCC), LV_STATE_DEFAULT);

        lv_obj_t * img_pay = lv_img_create(pay_btn);
        lv_img_set_src(img_pay, "S:/pay.png");
        lv_obj_center(img_pay);

        // 商品清单
        lv_obj_t * cart_list = lv_obj_create(pay_ui);
        lv_obj_set_size(cart_list, 293, 400);
        lv_obj_set_pos(cart_list, 100, 0);
        lv_obj_set_flex_flow(cart_list, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(cart_list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_all(cart_list, 5, 0);
        lv_obj_set_style_border_width(cart_list, 1, 0);
        lv_obj_set_style_border_color(cart_list, lv_color_hex(0xCCCCCC), 0);
        lv_obj_set_style_radius(cart_list, 8, 0);

        // 显示商品列表
        lv_obj_t * title_label = lv_label_create(cart_list);
        lv_label_set_text(title_label, "Payment Items:");
        lv_obj_set_style_text_font(title_label, &lv_font_montserrat_22, LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(title_label, lv_color_hex(0x333333), LV_STATE_DEFAULT);

        float total_price = 0;
        for(int i = 0; i < cart_item_count; i++) {

            char temp_price[32];
            strncpy(temp_price, shopping_cart[i].goods.price, sizeof(temp_price) - 1);
            temp_price[sizeof(temp_price) - 1] = '\0';
            
            lv_obj_t * item_cont               = lv_obj_create(cart_list);
            lv_obj_set_size(item_cont, 250, 80);
            lv_obj_set_flex_flow(item_cont, LV_FLEX_FLOW_COLUMN_WRAP);
            lv_obj_set_style_pad_all(item_cont, 2, 0);
            lv_obj_set_style_border_width(item_cont, 0, 0);

            // 商品名称
            lv_obj_t * name_label = lv_label_create(item_cont);
            lv_label_set_text(name_label, shopping_cart[i].goods.name);
            lv_label_set_long_mode(name_label, LV_LABEL_LONG_DOT);
            lv_obj_set_width(name_label, 150);
            lv_obj_set_style_text_font(name_label, &lv_font_montserrat_14, LV_STATE_DEFAULT);

            // 商品价格和数量
            char item_info[100];
            snprintf(item_info, sizeof(item_info), "%s x%d", shopping_cart[i].goods.price, shopping_cart[i].count);
            lv_obj_t * info_label = lv_label_create(item_cont);
            lv_label_set_text(info_label, item_info);
            lv_obj_set_style_text_font(info_label, &lv_font_montserrat_18, LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(info_label, lv_color_hex(0xFF0000), LV_STATE_DEFAULT);

            // 计算总价
            const char * c         = strtok(temp_price, " ");
            const char * pay_money = strtok(NULL, " ");
            total_price += atof(pay_money) * shopping_cart[i].count;
        }

        // 显示总价
        char total_text[50];
        snprintf(total_text, sizeof(total_text), "Total: $%.2f", total_price);
        lv_obj_t * total_label = lv_label_create(cart_list);
        lv_label_set_text(total_label, total_text);
        lv_obj_set_style_text_font(total_label, &lv_font_montserrat_22, LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(total_label, lv_color_hex(0xFF0000), LV_STATE_DEFAULT);

        // 退出支付按钮
        lv_obj_t * btn_pay_q = lv_btn_create(pay_ui);
        lv_obj_set_size(btn_pay_q, 100, 25);
        lv_obj_align(btn_pay_q, LV_ALIGN_BOTTOM_RIGHT, 10, -50);
        lv_obj_set_style_bg_color(btn_pay_q, lv_color_hex(0x00bfff), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(btn_pay_q, 255, LV_STATE_DEFAULT);

        lv_obj_t * lb_pay_q = lv_label_create(btn_pay_q);
        lv_label_set_text(lb_pay_q, "Quit");
        lv_obj_set_style_text_color(lb_pay_q, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_center(lb_pay_q);
        lv_obj_add_event_cb(btn_pay_q, btn_pay_q_cb, LV_EVENT_CLICKED, NULL);

        // 完成支付按钮
        lv_obj_t * btn_pay_fin = lv_btn_create(pay_ui);
        lv_obj_set_size(btn_pay_fin, 100, 25);
        lv_obj_align(btn_pay_fin, LV_ALIGN_BOTTOM_LEFT, 10, -50);
        lv_obj_set_style_bg_color(btn_pay_fin, lv_color_hex(0x00bfff), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(btn_pay_fin, 255, LV_STATE_DEFAULT);

        lv_obj_t * lb_pay_fin = lv_label_create(btn_pay_fin);
        lv_label_set_text(lb_pay_fin, "Finish");
        lv_obj_set_style_text_color(lb_pay_fin, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_center(lb_pay_fin);
        lv_obj_add_event_cb(btn_pay_fin, btn_pay_fin_cb, LV_EVENT_CLICKED, NULL);
    }
}
// 刷新购物车页面
void refresh_shopping_cart_page()
{
    if(tab3 == NULL) return;

    lv_obj_clean(tab3);

    // 创建清空购物车按钮
    lv_obj_t * btn_clear_car = lv_btn_create(tab3);
    lv_obj_set_size(btn_clear_car, 100, 25);
    lv_obj_align(btn_clear_car, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_set_style_bg_color(btn_clear_car, lv_color_hex(0x00bfff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn_clear_car, 255, LV_STATE_DEFAULT);

    lv_obj_t * lb_clear_car = lv_label_create(btn_clear_car);
    lv_label_set_text(lb_clear_car, "Clear Cart");
    lv_obj_set_style_text_color(lb_clear_car, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_center(lb_clear_car);

    lv_obj_add_event_cb(btn_clear_car, btn_clear_car_cb, LV_EVENT_CLICKED, NULL);

    // 创建支付按钮
    lv_obj_t * btn_pay = lv_btn_create(tab3);
    lv_obj_set_size(btn_pay, 100, 25);
    lv_obj_align(btn_pay, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_bg_color(btn_pay, lv_color_hex(0x00bfff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn_pay, 255, LV_STATE_DEFAULT);

    lv_obj_t * lb_pay = lv_label_create(btn_pay);
    lv_label_set_text(lb_pay, "Pay");
    lv_obj_set_style_text_color(lb_pay, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_center(lb_pay);

    lv_obj_add_event_cb(btn_pay, pay_car_cb, LV_EVENT_CLICKED, NULL);

    // 购物车为空提示
    if(cart_item_count == 0) {
        lv_obj_t * empty_label = lv_label_create(tab3);
        lv_label_set_text(empty_label, "Your shopping cart is empty!\nAdd some goods from Purchase page.");
        lv_obj_set_style_text_align(empty_label, LV_TEXT_ALIGN_CENTER, LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(empty_label, &lv_font_montserrat_18, LV_STATE_DEFAULT);
        lv_obj_center(empty_label);
        return;
    }

    // 渲染购物车商品
    lv_obj_t * cart_cont = lv_obj_create(tab3);
    lv_obj_set_size(cart_cont, 800, 400);
    lv_obj_align(cart_cont, LV_ALIGN_CENTER, 0, -35);
    lv_obj_set_flex_flow(cart_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cart_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    for(int i = 0; i < cart_item_count; i++) {
        lv_obj_t * item_cont = lv_obj_create(cart_cont);
        lv_obj_set_size(item_cont, 740, 80);
        lv_obj_set_flex_flow(item_cont, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(item_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_border_width(item_cont, 1, LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(item_cont, lv_color_hex(0xCCCCCC), LV_STATE_DEFAULT);
        lv_obj_set_style_radius(item_cont, 8, LV_STATE_DEFAULT);
        lv_obj_set_style_pad_all(item_cont, 5, LV_STATE_DEFAULT);

        // 商品图片
        lv_obj_t * item_img = lv_img_create(item_cont);
        lv_img_set_src(item_img, shopping_cart[i].goods.img_path);
        lv_obj_set_size(item_img, 60, 60);

        // 商品名称
        lv_obj_t * name_label = lv_label_create(item_cont);
        lv_label_set_text(name_label, shopping_cart[i].goods.name);
        lv_obj_set_width(name_label, 350);
        lv_label_set_long_mode(name_label, LV_LABEL_LONG_DOT);
        lv_obj_set_style_text_font(name_label, &lv_font_montserrat_12, LV_STATE_DEFAULT);

        // 商品价格
        lv_obj_t * price_label = lv_label_create(item_cont);
        lv_label_set_text(price_label, shopping_cart[i].goods.price);
        lv_obj_set_style_text_color(price_label, lv_color_hex(0xFF0000), LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(price_label, &lv_font_montserrat_16, LV_STATE_DEFAULT);

        // 购买数量
        lv_obj_t * count_label = lv_label_create(item_cont);
        char count_text[20];
        snprintf(count_text, sizeof(count_text), "Qty: %d", shopping_cart[i].count);
        lv_label_set_text(count_label, count_text);
        lv_obj_set_style_text_font(count_label, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    }
}

// 添加到购物车回调
void add_to_cart_btn_cb(lv_event_t * e)
{
    goods_item_t * goods = (goods_item_t *)lv_event_get_user_data(e);
    if(goods == NULL) return;

    int ret = add_goods_to_cart(goods);

    lv_obj_t * toast   = lv_label_create(lv_scr_act());
    char tip_text[100] = {0};
    if(ret == 0) {
        snprintf(tip_text, sizeof(tip_text), "Added: %s", goods->name);
    } else if(ret == 1) {
        snprintf(tip_text, sizeof(tip_text), "Qty +1: %s", goods->name);
    } else {
        lv_label_set_text(toast, "Shopping cart is full!");
    }
    if(ret != -1) lv_label_set_text(toast, tip_text);

    lv_obj_set_style_text_color(toast, lv_color_black(), LV_STATE_DEFAULT);
    lv_obj_align(toast, LV_ALIGN_TOP_MID, 20, 20);
    lv_obj_set_style_text_font(toast, &lv_font_montserrat_16, LV_STATE_DEFAULT);

    lv_timer_t * timer = lv_timer_create(toast_timer_cb, 2000, toast);

    refresh_shopping_cart_page();
}

// 标签页切换回调
void tabview_switch_cb(lv_event_t * e)
{
    lv_obj_t * tv    = lv_event_get_target(e);
    uint32_t tab_idx = lv_tabview_get_tab_act(tv);
    if(tab_idx == 2) {
        refresh_shopping_cart_page();
    }
}

// ========== 管理员商品管理功能 ==========
// 刷新管理员商品列表展示
void refresh_admin_goods_list()
{
    if(admin_goods_list_label == NULL) return;

    char list_text[4096] = "Admin Goods List (Separate from Purchase Page):\n";
    for(int i = 0; i < admin_goods_count; i++) {
        char item_text[200];
        snprintf(item_text, sizeof(item_text), "ID: %d | Name: %s | Price: %s\n", admin_goods_list[i].id,
                 admin_goods_list[i].name, admin_goods_list[i].price);
        strcat(list_text, item_text);
    }
    lv_label_set_text(admin_goods_list_label, list_text);
    lv_label_set_long_mode(admin_goods_list_label, LV_LABEL_LONG_WRAP);
}

// 管理员新增商品回调
void admin_add_goods_cb(lv_event_t * e)
{
    // 获取输入内容
    const char * id_str = lv_textarea_get_text(admin_ta_id);
    const char * name   = lv_textarea_get_text(admin_ta_name);
    const char * price  = lv_textarea_get_text(admin_ta_price);
    const char * img    = lv_textarea_get_text(admin_ta_img);

    // 输入校验
    if(id_str == NULL || strlen(id_str) == 0 || strlen(name) == 0 || strlen(price) == 0) {
        lv_obj_t * toast = lv_label_create(lv_scr_act());
        lv_label_set_text(toast, "Invalid input!");
        lv_obj_align(toast, LV_ALIGN_TOP_MID, 0, 20);
        lv_timer_create(toast_timer_cb, 2000, toast);
        return;
    }

    int id                 = atoi(id_str);
    goods_item_t new_goods = {0}; // 初始化结构体
    new_goods.id           = id;

    // 拷贝字符串到数组
    strncpy(new_goods.name, name, sizeof(new_goods.name) - 1);
    new_goods.name[sizeof(new_goods.name) - 1] = '\0';

    strncpy(new_goods.price, price, sizeof(new_goods.price) - 1);
    new_goods.price[sizeof(new_goods.price) - 1] = '\0';

    if(img && strlen(img) > 0) {
        strncpy(new_goods.img_path, img, sizeof(new_goods.img_path) - 1);
        new_goods.img_path[sizeof(new_goods.img_path) - 1] = '\0';
    } else {
        strcpy(new_goods.img_path, "S:/default.png");
    }
    int ret = admin_add_goods(new_goods);

    // 结果提示
    lv_obj_t * toast = lv_label_create(lv_scr_act());
    if(ret == 0) {
        lv_label_set_text(toast, "Add goods to admin list success!");
        // 清空表单
        lv_textarea_set_text(admin_ta_id, "");
        lv_textarea_set_text(admin_ta_name, "");
        lv_textarea_set_text(admin_ta_price, "");
        lv_textarea_set_text(admin_ta_img, "");
        // 刷新管理员列表
        refresh_admin_goods_list();
        // 注意：不刷新购买页面，保持两个列表独立
    } else if(ret == -1) {
        lv_label_set_text(toast, "Admin goods list full!");
    } else if(ret == -2) {
        lv_label_set_text(toast, "ID already exists in admin list!");
    }
    lv_obj_align(toast, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_style_text_font(toast, &lv_font_montserrat_16, LV_STATE_DEFAULT);
    lv_timer_create(toast_timer_cb, 2000, toast);
}

// 管理员删除商品回调
void admin_delete_goods_cb(lv_event_t * e)
{
    const char * id_str = lv_textarea_get_text(admin_ta_id);
    if(id_str == NULL || strlen(id_str) == 0) {
        lv_obj_t * toast = lv_label_create(lv_scr_act());
        lv_label_set_text(toast, "Please input goods ID!");
        lv_obj_align(toast, LV_ALIGN_TOP_MID, 0, 20);
        lv_timer_create(toast_timer_cb, 2000, toast);
        return;
    }

    int id  = atoi(id_str);
    int ret = admin_delete_goods(id);

    lv_obj_t * toast = lv_label_create(lv_scr_act());
    if(ret == 0) {
        lv_label_set_text(toast, "Delete from admin list success!");
        lv_textarea_set_text(admin_ta_id, "");
        refresh_admin_goods_list();
        // 注意：不刷新购买页面，保持两个列表独立
    } else {
        lv_label_set_text(toast, "Goods not found in admin list!");
    }
    lv_obj_align(toast, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_style_text_font(toast, &lv_font_montserrat_16, LV_STATE_DEFAULT);
    lv_timer_create(toast_timer_cb, 2000, toast);
}

// 管理员修改商品回调
void admin_update_goods_cb(lv_event_t * e)
{
    const char * id_str = lv_textarea_get_text(admin_ta_id);
    const char * name   = lv_textarea_get_text(admin_ta_name);
    const char * price  = lv_textarea_get_text(admin_ta_price);
    const char * img    = lv_textarea_get_text(admin_ta_img);

    if(id_str == NULL || strlen(id_str) == 0) {
        lv_obj_t * toast = lv_label_create(lv_scr_act());
        lv_label_set_text(toast, "Please input goods ID!");
        lv_obj_align(toast, LV_ALIGN_TOP_MID, 0, 20);
        lv_timer_create(toast_timer_cb, 2000, toast);
        return;
    }

    int id                   = atoi(id_str);
    goods_item_t * old_goods = admin_query_goods(id);
    if(old_goods == NULL) {
        lv_obj_t * toast = lv_label_create(lv_scr_act());
        lv_label_set_text(toast, "Goods not found in admin list!");
        lv_obj_align(toast, LV_ALIGN_TOP_MID, 0, 20);
        lv_timer_create(toast_timer_cb, 2000, toast);
        return;
    }

    // 用原有值填充空输入
    goods_item_t new_info = *old_goods;
    if(name && strlen(name) > 0) {
        strncpy(new_info.name, name, sizeof(new_info.name) - 1);
    }
    if(price && strlen(price) > 0) {
        strncpy(new_info.price, price, sizeof(new_info.price) - 1);
    }
    if(img && strlen(img) > 0) {
        strncpy(new_info.img_path, img, sizeof(new_info.img_path) - 1);
    }

    int ret = admin_update_goods(id, new_info);

    lv_obj_t * toast = lv_label_create(lv_scr_act());
    if(ret == 0) {
        lv_label_set_text(toast, "Update admin list success!");
        lv_textarea_set_text(admin_ta_id, "");
        lv_textarea_set_text(admin_ta_name, "");
        lv_textarea_set_text(admin_ta_price, "");
        lv_textarea_set_text(admin_ta_img, "");
        refresh_admin_goods_list();
        // 注意：不刷新购买页面，保持两个列表独立
    } else {
        lv_label_set_text(toast, "Update admin list failed!");
    }
    lv_obj_align(toast, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_style_text_font(toast, &lv_font_montserrat_16, LV_STATE_DEFAULT);
    lv_timer_create(toast_timer_cb, 2000, toast);
}

// ========== 购买页面动态渲染 ==========
void refresh_purchase_page()
{
    if(tab2 == NULL || !lv_obj_is_valid(tab2) || tab2_1 == NULL || !lv_obj_is_valid(tab2_1) || tab2_2 == NULL ||
       !lv_obj_is_valid(tab2_2) || tab2_3 == NULL || !lv_obj_is_valid(tab2_3)) {
        LV_LOG_WARN("Purchase page elements not initialized!");
        return;
    }

    // 清空原有商品
    lv_obj_clean(tab2_1);
    lv_obj_clean(tab2_2);
    lv_obj_clean(tab2_3);

    // 商品横向位置偏移
    int x_offset[]       = {20, 220, 420, 620, 820};
    int max_item_per_tab = sizeof(x_offset) / sizeof(x_offset[0]);

    // 分类渲染商品
    int monitor_idx = 0; // 显示器索引
    int host_idx    = 0; // 主机索引
    int gpu_idx     = 0;
    for(int i = 0; i < purchase_goods_count; i++) {
        goods_item_t * g      = &purchase_goods_list[i];
        lv_obj_t * parent_tab = NULL;
        int idx               = -1;

        // 按ID分类
        if(g->id >= 1001 && g->id <= 1005) { // 显示器
            if(monitor_idx >= max_item_per_tab) continue;
            parent_tab = tab2_1;
            idx        = monitor_idx++;
        } else if(g->id >= 1006 && g->id <= 1010) { // 主机
            if(host_idx >= max_item_per_tab) continue;
            parent_tab = tab2_2;
            idx        = host_idx++;
        } else if(g->id >= 1011 && g->id <= 1015) {
            if(gpu_idx >= max_item_per_tab) continue;
            parent_tab = tab2_3;
            idx        = gpu_idx++;
        } else {
            continue;
        }

        // 创建商品按钮
        lv_obj_t * btn_goods = lv_btn_create(parent_tab);
        lv_obj_set_size(btn_goods, 150, 150);
        lv_obj_set_pos(btn_goods, x_offset[idx], 60);

        // 商品图片
        lv_obj_t * img_goods = lv_img_create(btn_goods);
        lv_img_set_src(img_goods, g->img_path);
        lv_obj_set_size(img_goods, 150, 150);
        lv_obj_center(img_goods);

        // 价格标签
        lv_obj_t * lb_price = lv_label_create(parent_tab);
        lv_label_set_text(lb_price, g->price);
        lv_obj_set_size(lb_price, 150, 20);
        lv_obj_align_to(lb_price, btn_goods, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
        lv_obj_set_style_text_color(lb_price, lv_color_hex(0xff0000), LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(lb_price, &lv_font_montserrat_24, LV_STATE_DEFAULT);

        // 名称标签
        lv_obj_t * lb_name = lv_label_create(parent_tab);
        lv_label_set_text(lb_name, g->name);
        lv_label_set_long_mode(lb_name, LV_LABEL_LONG_WRAP);
        lv_obj_set_size(lb_name, 150, 100);
        lv_obj_align_to(lb_name, lb_price, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
        lv_obj_set_style_text_font(lb_name, &lv_font_montserrat_12, LV_STATE_DEFAULT);

        // 加入购物车按钮
        lv_obj_t * btn_add_cart = lv_btn_create(parent_tab);
        lv_obj_set_size(btn_add_cart, 150, 30);
        lv_obj_align_to(btn_add_cart, lb_name, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
        lv_obj_set_style_bg_color(btn_add_cart, lv_color_hex(0x00bfff), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(btn_add_cart, 255, LV_STATE_DEFAULT);

        lv_obj_t * lb_add = lv_label_create(btn_add_cart);
        lv_label_set_text(lb_add, "Add to Cart");
        lv_obj_set_style_text_color(lb_add, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_center(lb_add);

        lv_obj_add_event_cb(btn_add_cart, add_to_cart_btn_cb, LV_EVENT_CLICKED, g);
    }
}

// ========== 原有UI功能函数 ==========
// 字体创建函数
lv_style_t * my_lv_create_style(const char * fontpath, int fontsize)
{
    static lv_ft_info_t info;
    info.name   = fontpath;
    info.weight = fontsize;
    info.style  = FT_FONT_STYLE_NORMAL;
    info.mem    = NULL;

    if(!lv_ft_font_init(&info)) {
        LV_LOG_ERROR("Font create failed\n");
        return NULL;
    }

    lv_style_t * style = malloc(sizeof(lv_style_t));
    if(style == NULL) return NULL;

    lv_style_init(style);
    lv_style_set_text_font(style, info.font);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    return style;
}

// 文本框聚焦事件
void ta1_cb(lv_event_t * e)
{
    lv_keyboard_set_textarea(kb, ta1);
    lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_refr_now(lv_disp_get_default());
}

void ta2_cb(lv_event_t * e)
{
    lv_keyboard_set_textarea(kb, ta2);
    lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_refr_now(lv_disp_get_default());
}

// 消息盒子
void bt_msgbox_cb(lv_event_t * e)
{
    const char * btns[] = {"", "", ""};
    lv_obj_t * msgbox   = lv_msgbox_create(page1, "Warning",
                                           "The password/account you entered is incorrect.Please click the 'reg' button "
                                             "to complete the registration process.",
                                           btns, true);
    lv_obj_set_pos(msgbox, 260, 80);
}

void bt_msgbox_cb2(lv_event_t * e)
{
    const char * btns[] = {"", "", ""};
    lv_obj_t * msgbox   = lv_msgbox_create(page1, "Warning",
                                           "Name or password is invalid.",
                                           btns, true);
    lv_obj_set_pos(msgbox, 260, 80);
}

void bt_msgbox_cb3(const char *name)
{
    char msg_buf[200];
    snprintf(msg_buf, sizeof(msg_buf), "Username %s is already registered!", name);

    const char * btns[] = {"", "", ""};
    lv_obj_t * msgbox   = lv_msgbox_create(page1, "Warning",msg_buf, btns, true);
    lv_obj_set_pos(msgbox, 260, 80);
}

void bt_msgbox_cb4(const char *name)
{
    char msg_buf[200];
    snprintf(msg_buf, sizeof(msg_buf), "Register success! Name: %s\n", name);

    const char * btns[] = {"", "", ""};
    lv_obj_t * msgbox   = lv_msgbox_create(page1, "Warning",msg_buf, btns, true);
    lv_obj_set_pos(msgbox, 260, 80);
}


int my_write(const char *name, const char *passwd)
{
    // 参数合法性检查
    if(name == NULL || passwd == NULL || strlen(name) == 0 || strlen(passwd) == 0) {
        bt_msgbox_cb2(NULL);
        return -1;
    }

    // 1. 检查用户名是否已存在
    FILE * myfile = fopen("./test.txt", "r");
    if(myfile != NULL) { // 文件存在时检查
        char buf[50] = {0};
        int name_exist = 0;

        while(fgets(buf, sizeof(buf), myfile) != NULL) {
            // 去除换行符
            buf[strcspn(buf, "\n")] = '\0';
            
            // 切割用户名
            char * p = strtok(buf, "@");
            if(p != NULL && strcmp(p, name) == 0) {
                name_exist = 1;
                break;
            }
        }

        fclose(myfile);

        // 用户名已存在，返回错误
        if(name_exist) {
            bt_msgbox_cb3(name);
            return -1;
        }
    }

    // 2. 追加写入新用户信息
    myfile = fopen("./test.txt", "a+");
    if(myfile == NULL) {
        perror("Open file failed");
        return -1;
    }

    // 拼接用户名和密码
    char msg[100] = {0};
    sprintf(msg, "%s@%s", name, passwd);

    // 写入（先换行，避免和最后一行粘连）
    fwrite("\n", 1, 1, myfile);
    fwrite(msg, 1, strlen(msg), myfile);

    fclose(myfile);
    bt_msgbox_cb4(name);

    return 0;
}

// 登录按钮回调
void bt_login_cb(lv_event_t * e)
{
    char name[50]   = {0};
    char passwd[50] = {0};
    strncpy(name, lv_textarea_get_text(ta1), sizeof(name) - 1);
    strncpy(passwd, lv_textarea_get_text(ta2), sizeof(passwd) - 1);

    if(strlen(name) == 0 || strlen(passwd) == 0) {
        return;
    }

    int login_ok  = 0;
    FILE * myfile = fopen("./test.txt", "r");
    if(myfile == NULL) {
        perror("Open file failed");
        return;
    }

    char buf[50] = {0};
    while(fgets(buf, sizeof(buf), myfile) != NULL) {
        buf[strcspn(buf, "\n")] = '\0';

        char temp_buf[50];
        strncpy(temp_buf, buf, sizeof(temp_buf) - 1);
        char * p_name = strtok(temp_buf, "@");
        char * p_pass = strtok(NULL, "@");

        if(p_name && p_pass && strcmp(p_name, name) == 0 && strcmp(p_pass, passwd) == 0) {
            login_ok = 1;
            break;
        }
    }

    fclose(myfile);

    if(login_ok) {
        show_login_page2();
        lv_obj_del(page1);
    } else {
        bt_msgbox_cb(NULL);
    }
}

// 注册按钮回调
void bt_regt_cb(lv_event_t * e)
{
    char name[50]   = {0};
    char passwd[50] = {0};
    strncpy(name, lv_textarea_get_text(ta1), sizeof(name) - 1);
    strncpy(passwd, lv_textarea_get_text(ta2), sizeof(passwd) - 1);

    if(strlen(name) == 0 || strlen(passwd) == 0) {
        LV_LOG_WARN("Name or password is empty");
        return;
    }

    my_write(name, passwd);

    lv_textarea_set_text(ta1, "");
    lv_textarea_set_text(ta2, "");
}

// 键盘退出按钮回调
void bt_key_cb(lv_event_t * e)
{
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
}

// 返回登录页按钮回调
void bt_admin_cb(lv_event_t * e)
{
    show_sec_page1();
    lv_obj_del(page2);
}

// 登录页面1
void show_login_page1()
{
    page1 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(page1, 800, 480);
    lv_obj_set_pos(page1, 0, 0);

    lv_obj_t * img = lv_img_create(page1);
    LV_IMG_DECLARE(image1);
    lv_img_set_src(img, &image1);
    lv_obj_align_to(img, page1, LV_ALIGN_CENTER, 0, 0);

    // 退出键盘按钮
    bt_key            = lv_btn_create(page1);
    lv_obj_t * lb_key = lv_label_create(bt_key);
    lv_label_set_text(lb_key, "quit");
    lv_obj_set_size(bt_key, 60, 30);
    lv_obj_set_pos(bt_key, 0, 200);
    lv_obj_add_event_cb(bt_key, bt_key_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(bt_key, lv_color_hex(0xFFFFFF), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bt_key, 255, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lb_key, lv_color_hex(0x000000), LV_STATE_DEFAULT);

    // 登录/注册按钮
    bt1 = lv_btn_create(page1);
    bt2 = lv_btn_create(page1);
    lv_obj_set_size(bt1, 60, 30);
    lv_obj_set_pos(bt1, 300, 280);
    lv_obj_set_size(bt2, 60, 30);
    lv_obj_align_to(bt2, bt1, LV_ALIGN_OUT_RIGHT_MID, 70, 0);
    lv_obj_set_style_bg_color(bt1, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bt1, 255, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(bt2, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bt2, 255, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(bt1, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bt1, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(bt1, lv_color_hex(0xCCCCCC), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(bt2, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bt2, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(bt2, lv_color_hex(0xCCCCCC), LV_STATE_DEFAULT);

    // 按钮文字
    lv_obj_t * lb1 = lv_label_create(bt1);
    lv_label_set_text(lb1, "login");
    lv_obj_t * lb2 = lv_label_create(bt2);
    lv_label_set_text(lb2, "regt");
    lv_obj_set_style_text_color(lb1, lv_color_hex(0x000000), LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lb2, lv_color_hex(0x000000), LV_STATE_DEFAULT);
    lv_obj_align_to(lb1, bt1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align_to(lb2, bt2, LV_ALIGN_CENTER, 0, 0);

    // 标题
    lv_obj_t * lb_title = lv_label_create(page1);
    lv_label_set_text(lb_title, "Welcome Back");
    lv_obj_align(lb_title, LV_ALIGN_CENTER, 0, -200);
    lv_obj_set_style_text_font(lb_title, &lv_font_montserrat_36, LV_STATE_DEFAULT);

    lv_obj_t * lb_title1 = lv_label_create(page1);
    lv_label_set_text(lb_title1, "------------------LOGIN WITH EMAIL------------------");
    lv_obj_align_to(lb_title1, lb_title, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_style_text_font(lb_title1, &lv_font_montserrat_12, LV_STATE_DEFAULT);

    // 账户/密码标签
    lv_obj_t * lb3 = lv_label_create(page1);
    lv_label_set_text(lb3, "Address");
    lv_obj_t * lb4 = lv_label_create(page1);
    lv_label_set_text(lb4, "password");
    lv_obj_set_pos(lb3, 250, 90);
    lv_obj_set_pos(lb4, 250, 170);

    // 文本输入框
    ta1 = lv_textarea_create(page1);
    ta2 = lv_textarea_create(page1);
    lv_obj_set_size(ta1, 300, 50);
    lv_obj_set_pos(ta1, 250, 110);
    lv_obj_set_size(ta2, 300, 50);
    lv_obj_set_pos(ta2, 250, 190);
    lv_textarea_set_one_line(ta1, true);
    lv_textarea_set_one_line(ta2, true);
    lv_textarea_set_password_mode(ta2, true);
    lv_textarea_set_password_show_time(ta2, 1000);

    // 软键盘
    kb = lv_keyboard_create(lv_scr_act());
    lv_obj_set_size(kb, 800, 200);
    lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

    // 绑定事件
    lv_obj_add_event_cb(ta1, ta1_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(ta2, ta2_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(bt1, bt_login_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(bt2, bt_regt_cb, LV_EVENT_CLICKED, NULL);
}

// 二级页面文本框事件
void ta1_sec_cb(lv_event_t * e)
{
    lv_keyboard_set_textarea(sec_kb1, sec_ta1);
    lv_obj_clear_flag(sec_kb1, LV_OBJ_FLAG_HIDDEN);
    lv_refr_now(lv_disp_get_default());
}

void ta2_sec_cb(lv_event_t * e)
{
    lv_keyboard_set_textarea(sec_kb1, sec_ta2);
    lv_obj_clear_flag(sec_kb1, LV_OBJ_FLAG_HIDDEN);
    lv_refr_now(lv_disp_get_default());
}
void bt_sec_key_cb(lv_event_t * e)
{
    lv_obj_add_flag(sec_kb1, LV_OBJ_FLAG_HIDDEN);
}

// 管理员消息盒子
void bt_sec_msgbox_cb(lv_event_t * e)
{
    const char * btns[] = {"", "", ""};
    lv_obj_t * sec_msgbox =
        lv_msgbox_create(sec_page1, "Warning", "The password/account you entered is incorrect.", btns, true);
    lv_obj_set_pos(sec_msgbox, 260, 80);
}

// 管理员登录回调
void bt_sec_login_cb(lv_event_t * e)
{
    const char * ad_name = lv_textarea_get_text(sec_ta1);
    const char * ad_pwd  = lv_textarea_get_text(sec_ta2);

    if(ad_name == NULL) ad_name = "";
    if(ad_pwd == NULL) ad_pwd = "";

    if(strcmp(ad_name, "yyd") == 0 && strcmp(ad_pwd, "123456") == 0) {
        show_sec_page2();
        lv_obj_del(sec_page1);
    } else {
        bt_sec_msgbox_cb(NULL);
    }
}

// 管理员退出回调
void bt_sec_quit_cb(lv_event_t * e)
{
    show_login_page2();
    lv_obj_del(sec_page1);
    refresh_shopping_cart_page();
}

void admin_quit_cb(lv_event_t * e)
{
    show_login_page2();
    lv_obj_del(sec_page2);
    refresh_purchase_page();
    refresh_shopping_cart_page();
}

// 商品管理页面
void admin_ta_id_cb(lv_event_t * e)
{
    lv_keyboard_set_textarea(sec_kb2, admin_ta_id);
    lv_obj_clear_flag(sec_kb2, LV_OBJ_FLAG_HIDDEN);
    lv_refr_now(lv_disp_get_default());
}
void admin_ta_name_cb(lv_event_t * e)
{
    lv_keyboard_set_textarea(sec_kb2, admin_ta_name);
    lv_obj_clear_flag(sec_kb2, LV_OBJ_FLAG_HIDDEN);
    lv_refr_now(lv_disp_get_default());
}
void admin_ta_price_cb(lv_event_t * e)
{
    lv_keyboard_set_textarea(sec_kb2, admin_ta_price);
    lv_obj_clear_flag(sec_kb2, LV_OBJ_FLAG_HIDDEN);
    lv_refr_now(lv_disp_get_default());
}
void admin_ta_img_cb(lv_event_t * e)
{
    lv_keyboard_set_textarea(sec_kb2, admin_ta_img);
    lv_obj_clear_flag(sec_kb2, LV_OBJ_FLAG_HIDDEN);
    lv_refr_now(lv_disp_get_default());
}
void bt_sec_key_cb2(lv_event_t * e)
{
    lv_obj_add_flag(sec_kb2, LV_OBJ_FLAG_HIDDEN);
}

void show_sec_page2()
{
    // ========== 商品管理模块 ==========
    // 商品管理标题
    sec_page2 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(sec_page2, 800, 800);

    lv_obj_t * admin_title = lv_label_create(sec_page2);
    lv_label_set_text(admin_title, "Goods Management (Separate List)");
    lv_obj_align(admin_title, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(admin_title, &lv_font_montserrat_28, LV_STATE_DEFAULT);

    // 表单标签
    lv_obj_t * lb_id = lv_label_create(sec_page2);
    lv_label_set_text(lb_id, "Goods ID:");
    lv_obj_align(lb_id, LV_ALIGN_TOP_LEFT, 80, 120);

    lv_obj_t * lb_name = lv_label_create(sec_page2);
    lv_label_set_text(lb_name, "Name:");
    lv_obj_align(lb_name, LV_ALIGN_TOP_LEFT, 100, 180);

    lv_obj_t * lb_price = lv_label_create(sec_page2);
    lv_label_set_text(lb_price, "Price:");
    lv_obj_align(lb_price, LV_ALIGN_TOP_LEFT, 105, 240);

    lv_obj_t * lb_img = lv_label_create(sec_page2);
    lv_label_set_text(lb_img, "Img Path:");
    lv_obj_align(lb_img, LV_ALIGN_TOP_LEFT, 75, 300);

    // 表单输入框
    admin_ta_id = lv_textarea_create(sec_page2);
    lv_obj_set_size(admin_ta_id, 200, 40);
    lv_obj_align_to(admin_ta_id, lb_id, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_textarea_set_one_line(admin_ta_id, true);

    admin_ta_name = lv_textarea_create(sec_page2);
    lv_obj_set_size(admin_ta_name, 200, 40);
    lv_obj_align_to(admin_ta_name, lb_name, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_textarea_set_one_line(admin_ta_name, true);

    admin_ta_price = lv_textarea_create(sec_page2);
    lv_obj_set_size(admin_ta_price, 200, 40);
    lv_obj_align_to(admin_ta_price, lb_price, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_textarea_set_one_line(admin_ta_price, true);

    admin_ta_img = lv_textarea_create(sec_page2);
    lv_obj_set_size(admin_ta_img, 200, 40);
    lv_obj_align_to(admin_ta_img, lb_img, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_textarea_set_one_line(admin_ta_img, true);

    // 退出按钮
    lv_obj_t * btn_quit = lv_btn_create(sec_page2);
    lv_obj_set_size(btn_quit, 80, 30);
    lv_obj_align(btn_quit, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_t * lb_quit = lv_label_create(btn_quit);
    lv_label_set_text(lb_quit, "quit");
    lv_obj_center(lb_quit);
    lv_obj_add_event_cb(btn_quit, admin_quit_cb, LV_EVENT_CLICKED, NULL);

    // 退出键盘按钮
    bt_key2            = lv_btn_create(sec_page2);
    lv_obj_t * lb_key2 = lv_label_create(bt_key2);
    lv_label_set_text(lb_key2, "quit");
    lv_obj_set_size(bt_key2, 60, 30);
    lv_obj_set_pos(bt_key2, 0, 200);
    lv_obj_add_event_cb(bt_key2, bt_sec_key_cb2, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(bt_key2, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bt_key2, 255, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lb_key2, lv_color_hex(0x000000), LV_STATE_DEFAULT);

    // 操作按钮
    lv_obj_t * btn_add = lv_btn_create(sec_page2);
    lv_obj_set_size(btn_add, 80, 30);
    lv_obj_align(btn_add, LV_ALIGN_TOP_RIGHT, -170, 140);
    lv_obj_t * lb_add = lv_label_create(btn_add);
    lv_label_set_text(lb_add, "Add");
    lv_obj_center(lb_add);
    lv_obj_add_event_cb(btn_add, admin_add_goods_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * btn_del = lv_btn_create(sec_page2);
    lv_obj_set_size(btn_del, 80, 30);
    lv_obj_align_to(btn_del, btn_add, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
    lv_obj_t * lb_del = lv_label_create(btn_del);
    lv_label_set_text(lb_del, "Delete");
    lv_obj_center(lb_del);
    lv_obj_add_event_cb(btn_del, admin_delete_goods_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * btn_update = lv_btn_create(sec_page2);
    lv_obj_set_size(btn_update, 80, 30);
    lv_obj_align_to(btn_update, btn_del, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
    lv_obj_t * lb_update = lv_label_create(btn_update);
    lv_label_set_text(lb_update, "Update");
    lv_obj_center(lb_update);
    lv_obj_add_event_cb(btn_update, admin_update_goods_cb, LV_EVENT_CLICKED, NULL);

    // 商品列表展示
    admin_goods_list_label = lv_label_create(sec_page2);
    lv_obj_set_size(admin_goods_list_label, 700, 200);
    lv_obj_align(admin_goods_list_label, LV_ALIGN_BOTTOM_MID, 60, -150);
    lv_obj_set_style_text_font(admin_goods_list_label, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    refresh_admin_goods_list();

    // 软键盘
    sec_kb2 = lv_keyboard_create(lv_scr_act());
    lv_obj_set_size(sec_kb2, 800, 200);
    lv_obj_align(sec_kb2, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(sec_kb2, LV_OBJ_FLAG_HIDDEN);

    // 绑定事件
    lv_obj_add_event_cb(admin_ta_id, admin_ta_id_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(admin_ta_name, admin_ta_name_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(admin_ta_price, admin_ta_price_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(admin_ta_img, admin_ta_img_cb, LV_EVENT_FOCUSED, NULL);
}

// 管理员页面
void show_sec_page1()
{
    sec_page1 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(sec_page1, 800, 480);

    // 退出键盘按钮
    bt_key            = lv_btn_create(sec_page1);
    lv_obj_t * lb_key = lv_label_create(bt_key);
    lv_label_set_text(lb_key, "quit");
    lv_obj_set_size(bt_key, 60, 30);
    lv_obj_set_pos(bt_key, 0, 200);
    lv_obj_add_event_cb(bt_key, bt_sec_key_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(bt_key, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bt_key, 255, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lb_key, lv_color_hex(0x000000), LV_STATE_DEFAULT);

    // 登录/退出按钮
    sec_bt1 = lv_btn_create(sec_page1);
    sec_bt2 = lv_btn_create(sec_page1);
    lv_obj_set_size(sec_bt1, 60, 30);
    lv_obj_set_pos(sec_bt1, 300, 280);
    lv_obj_set_size(sec_bt2, 60, 30);
    lv_obj_align_to(sec_bt2, sec_bt1, LV_ALIGN_OUT_RIGHT_MID, 70, 0);
    lv_obj_set_style_bg_color(sec_bt1, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(sec_bt1, 255, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(sec_bt2, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(sec_bt2, 255, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(sec_bt1, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(sec_bt1, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(sec_bt1, lv_color_hex(0xCCCCCC), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(sec_bt2, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(sec_bt2, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(sec_bt2, lv_color_hex(0xCCCCCC), LV_STATE_DEFAULT);

    // 按钮文字
    lv_obj_t * lb_sec1 = lv_label_create(sec_bt1);
    lv_label_set_text(lb_sec1, "login");
    lv_obj_t * lb_sec2 = lv_label_create(sec_bt2);
    lv_label_set_text(lb_sec2, "quit");
    lv_obj_set_style_text_color(lb_sec1, lv_color_hex(0x000000), LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lb_sec2, lv_color_hex(0x000000), LV_STATE_DEFAULT);
    lv_obj_align_to(lb_sec1, sec_bt1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align_to(lb_sec2, sec_bt2, LV_ALIGN_CENTER, 0, 0);

    // 标题
    lv_obj_t * lb_sec_title = lv_label_create(sec_page1);
    lv_label_set_text(lb_sec_title, "Administrator Center");
    lv_obj_align(lb_sec_title, LV_ALIGN_CENTER, 0, -200);
    lv_obj_set_style_text_font(lb_sec_title, &lv_font_montserrat_36, LV_STATE_DEFAULT);

    // 账户/密码标签
    lv_obj_t * lb_sec3 = lv_label_create(sec_page1);
    lv_label_set_text(lb_sec3, "Admin Address");
    lv_obj_t * lb_sec4 = lv_label_create(sec_page1);
    lv_label_set_text(lb_sec4, "Admin Password");
    lv_obj_set_pos(lb_sec3, 250, 90);
    lv_obj_set_pos(lb_sec4, 250, 170);

    // 文本输入框
    sec_ta1 = lv_textarea_create(sec_page1);
    sec_ta2 = lv_textarea_create(sec_page1);
    lv_obj_set_size(sec_ta1, 300, 50);
    lv_obj_set_pos(sec_ta1, 250, 110);
    lv_obj_set_size(sec_ta2, 300, 50);
    lv_obj_set_pos(sec_ta2, 250, 190);
    lv_textarea_set_password_mode(sec_ta2, true);
    lv_textarea_set_password_show_time(sec_ta2, 1000);
    lv_textarea_set_one_line(sec_ta1, true);
    lv_textarea_set_one_line(sec_ta2, true);

    // 软键盘
    sec_kb1 = lv_keyboard_create(sec_page1);
    lv_obj_set_size(sec_kb1, 800, 200);
    lv_obj_align(sec_kb1, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(sec_kb1, LV_OBJ_FLAG_HIDDEN);

    // 绑定事件
    lv_obj_add_event_cb(sec_ta1, ta1_sec_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(sec_ta2, ta2_sec_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(sec_bt1, bt_sec_login_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(sec_bt2, bt_sec_quit_cb, LV_EVENT_CLICKED, NULL);
}

// 登录页面2（主商城页面）
void show_login_page2()
{
    page2 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(page2, 800, 480);
    lv_obj_set_style_pad_all(page2, 0, 0);
    lv_obj_set_style_border_width(page2, 0, 0);

    lv_obj_t * tabview = lv_tabview_create(page2, LV_DIR_BOTTOM, 50);
    lv_obj_set_size(tabview, 800, 480);
    tab1 = lv_tabview_add_tab(tabview, "Home Page");
    tab2 = lv_tabview_add_tab(tabview, "Purchase");
    tab3 = lv_tabview_add_tab(tabview, "Shopping Car");
    tab4 = lv_tabview_add_tab(tabview, "Personal Center");
    lv_tabview_set_act(tabview, 0, LV_ANIM_OFF);

    lv_obj_add_event_cb(tabview, tabview_switch_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 主页内容
    lv_obj_t * btn_home1 = lv_btn_create(tab1);
    lv_obj_set_size(btn_home1, 500, 245);
    lv_obj_set_pos(btn_home1, 40, 100);

    lv_obj_t * img_home1 = lv_img_create(btn_home1);
    lv_img_set_src(img_home1, "S:/home.png");
    lv_obj_set_size(img_home1, 500, 245);
    lv_obj_set_style_radius(img_home1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_center(img_home1);

    lv_obj_t * btn_home2 = lv_btn_create(tab1);
    lv_obj_set_size(btn_home2, 200, 66);
    lv_obj_set_pos(btn_home2, 560, 95);

    lv_obj_t * img_home2 = lv_img_create(btn_home2);
    lv_img_set_src(img_home2, "S:/home1.png");
    lv_obj_set_size(img_home2, 200, 66);
    lv_obj_set_style_radius(img_home2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_center(img_home2);

    lv_obj_t * btn_home3 = lv_btn_create(tab1);
    lv_obj_set_size(btn_home3, 200, 66);
    lv_obj_set_pos(btn_home3, 560, 185);

    lv_obj_t * img_home3 = lv_img_create(btn_home3);
    lv_img_set_src(img_home3, "S:/home2.png");
    lv_obj_set_size(img_home3, 200, 66);
    lv_obj_set_style_radius(img_home3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_center(img_home3);

    lv_obj_t * btn_home4 = lv_btn_create(tab1);
    lv_obj_set_size(btn_home4, 200, 66);
    lv_obj_set_pos(btn_home4, 560, 285);

    lv_obj_t * img_home4 = lv_img_create(btn_home4);
    lv_img_set_src(img_home4, "S:/home3.png");
    lv_obj_set_size(img_home4, 200, 66);
    lv_obj_set_style_radius(img_home4, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_center(img_home4);

    lv_obj_t * lb_home5 = lv_label_create(tab1);
    lv_label_set_text(lb_home5, "Computers / Mobile phones / Food / Beauty products / Household appliances / Men's "
                                "clothing / Women's clothing");
    lv_obj_set_size(lb_home5, 700, 40);
    lv_obj_set_pos(lb_home5, 30, 370);
    lv_obj_set_style_text_color(lb_home5, lv_color_hex(0x000000), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lb_home5, &lv_font_montserrat_16, LV_STATE_DEFAULT);

    lv_obj_set_style_pad_all(tab1, 0, 0);
    lv_obj_set_style_border_width(tab1, 0, 0);
    lv_obj_t * my_lb1 = lv_label_create(tab1);
    lv_label_set_text(my_lb1, "Welcome to the online sales platform of yyd. ");
    lv_obj_set_size(my_lb1, 800, 40);
    lv_obj_set_pos(my_lb1, 40, 0);
    lv_obj_set_style_text_color(my_lb1, lv_color_hex(0x000000), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(my_lb1, &lv_font_montserrat_24, LV_STATE_DEFAULT);
    lv_obj_t * my_lb2 = lv_label_create(tab1);
    lv_label_set_text(my_lb2, "Welcome to make your purchase!");
    lv_obj_set_size(my_lb1, 800, 40);
    lv_obj_align_to(my_lb2, my_lb1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_text_color(my_lb2, lv_color_hex(0x000000), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(my_lb2, &lv_font_montserrat_24, LV_STATE_DEFAULT);

    // 购买页面子标签
    lv_obj_t * tabview2 = lv_tabview_create(tab2, LV_DIR_LEFT, 100);
    lv_obj_set_size(tabview2, 800, 480);
    lv_obj_set_style_pad_all(tab2, 0, 0);
    lv_obj_set_style_border_width(tab2, 0, 0);
    tab2_1 = lv_tabview_add_tab(tabview2, "Display screen");
    tab2_2 = lv_tabview_add_tab(tabview2, "Host machine");
    tab2_3 = lv_tabview_add_tab(tabview2, "Graphics card");
    lv_tabview_set_act(tabview2, 0, LV_ANIM_OFF);

    // 初始化默认商品并动态渲染购买页面
    init_default_purchase_goods();
    init_default_admin_goods();
    refresh_purchase_page();
    refresh_shopping_cart_page();

    // 个人中心页面
    lv_obj_t * btn_user = lv_btn_create(tab4);
    lv_obj_set_size(btn_user, 100, 100);
    lv_obj_set_pos(btn_user, 10, 10);
    lv_obj_remove_style_all(btn_user);

    lv_obj_t * img1 = lv_img_create(btn_user);
    lv_img_set_src(img1, "S:/zhou.jpg");
    lv_obj_set_size(img1, 90, 90);
    lv_obj_set_style_radius(img1, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align_to(img1, btn_user, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * lb_c1 = lv_label_create(tab4);
    lv_label_set_text(lb_c1, "Username: ");
    lv_obj_align_to(lb_c1, btn_user, LV_ALIGN_OUT_RIGHT_TOP, 40, 0);
    lv_obj_set_style_text_font(lb_c1, &lv_font_montserrat_18, LV_STATE_DEFAULT);
    lv_obj_t * lb_c2 = lv_label_create(tab4);
    lv_label_set_text(lb_c2, "Membership status: ");
    lv_obj_align_to(lb_c2, btn_user, LV_ALIGN_OUT_RIGHT_BOTTOM, 40, 0);
    lv_obj_set_style_text_font(lb_c2, &lv_font_montserrat_18, LV_STATE_DEFAULT);
    lv_obj_t * lb_c3 = lv_label_create(tab4);
    lv_label_set_text(lb_c3, "Personal points: ");
    lv_obj_align_to(lb_c3, btn_user, LV_ALIGN_OUT_RIGHT_MID, 40, 0);
    lv_obj_set_style_text_font(lb_c3, &lv_font_montserrat_18, LV_STATE_DEFAULT);

    lv_obj_t * bt_q = lv_btn_create(tab4);
    lv_obj_set_size(bt_q, 100, 25);
    lv_obj_align(bt_q, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_set_style_bg_color(bt_q, lv_color_hex(0x00bfff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bt_q, 255, LV_STATE_DEFAULT);

    lv_obj_t * lb_q = lv_label_create(bt_q);
    lv_label_set_text(lb_q, "Admin");
    lv_obj_set_style_text_color(lb_q, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_center(lb_q);

    lv_obj_add_event_cb(bt_q, bt_admin_cb, LV_EVENT_CLICKED, NULL);
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
