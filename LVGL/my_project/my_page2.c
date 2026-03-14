#include "myhead.h"

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

void toast_timer_cb1(lv_timer_t * timer)
{
    lv_obj_t * toast = (lv_obj_t *)timer->user_data;
    if(toast && lv_obj_is_valid(toast)) {
        lv_obj_del(toast);
    }
    lv_timer_del(timer);
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

    lv_timer_t * timer = lv_timer_create(toast_timer_cb1, 2000, toast);

    refresh_shopping_cart_page();
}

// 清空购物车回调
void btn_clear_car_cb(lv_event_t * e)
{
    cart_item_count = 0;
    memset(shopping_cart, 0, sizeof(cart_item_t) * MAX_CART_ITEMS);

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

    lv_timer_t * timer = lv_timer_create(toast_timer_cb1, 2000, toast);

    refresh_shopping_cart_page();
}

// 支付UI对象
lv_obj_t * pay_ui = NULL;
lv_obj_t * img_pay_btn = NULL;  // 支付图片按钮
bool is_first_pay_method = true;  // 当前支付方式（true为第一种，false为第二种）

// 支付图片点击回调
void pay_image_click_cb(lv_event_t * e)
{
    if(img_pay_btn && lv_obj_is_valid(img_pay_btn)) {
        if(is_first_pay_method) {
            // 切换到第二种支付方式
            lv_img_set_src(img_pay_btn, "S:/pay2.png");  // 第二张支付图片
            is_first_pay_method = false;
        } else {
            // 切换回第一种支付方式
            lv_img_set_src(img_pay_btn, "S:/pay.png");  // 第一张支付图片
            is_first_pay_method = true;
        }
    }
}

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
    
    // 重置支付方式为第一种
    is_first_pay_method = true;
    
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
    
    // 重置支付方式为第一种
    is_first_pay_method = true;
    
    // 清空购物车
    cart_item_count = 0;
    memset(shopping_cart, 0, sizeof(cart_item_t) * MAX_CART_ITEMS);
    
    // 返回主页面
    show_login_page2();
    
    // 显示支付成功提示
    lv_obj_t * toast = lv_label_create(lv_scr_act());
    lv_label_set_text(toast, "Payment successful! Cart cleared.");
    lv_obj_set_style_text_color(toast, lv_color_black(), LV_STATE_DEFAULT);
    lv_obj_align(toast, LV_ALIGN_TOP_MID, 20, 20);
    lv_obj_set_style_text_font(toast, &lv_font_montserrat_16, LV_STATE_DEFAULT);
    fun();
    
    lv_timer_t * timer = lv_timer_create(toast_timer_cb1, 2000, toast);
    
    refresh_shopping_cart_page();
}

// 返回登录页按钮回调
void bt_admin_cb(lv_event_t * e)
{
    show_sec_page1();
    lv_obj_del(page2);
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
        
        // 支付二维码图片按钮
        lv_obj_t * pay_btn = lv_btn_create(pay_ui);
        lv_obj_set_size(pay_btn, 293, 400);
        lv_obj_set_pos(pay_btn, 400, 0);
        lv_obj_set_style_radius(pay_btn, 8, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(pay_btn, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(pay_btn, 1, LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(pay_btn, lv_color_hex(0xCCCCCC), LV_STATE_DEFAULT);
        
        // 添加点击事件
        lv_obj_add_event_cb(pay_btn, pay_image_click_cb, LV_EVENT_CLICKED, NULL);
        
        // 创建支付图片
        img_pay_btn = lv_img_create(pay_btn);
        lv_img_set_src(img_pay_btn, "S:/pay.png");  // 默认显示第一张支付图片
        lv_obj_center(img_pay_btn);
        
        // 添加点击提示文字
        lv_obj_t * hint_label = lv_label_create(pay_ui);
        lv_label_set_text(hint_label, "Click to change payment method");
        lv_obj_set_style_text_font(hint_label, &lv_font_montserrat_14, LV_STATE_DEFAULT);
        lv_obj_align_to(hint_label, pay_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
        
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
            
            lv_obj_t * item_cont = lv_obj_create(cart_list);
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


// 标签页切换回调
void tabview_switch_cb(lv_event_t * e)
{
    lv_obj_t * tv    = lv_event_get_target(e);
    uint32_t tab_idx = lv_tabview_get_tab_act(tv);
    if(tab_idx == 2) {
        refresh_shopping_cart_page();
    }
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
    strcpy(g6.name, "ASUS ROG All-in-One Gaming Computer System U7 265KF");
    strcpy(g6.price, "$ 13499");
    strcpy(g6.img_path, "S:/zj2.png");
    purchase_goods_list[purchase_goods_count++] = g6;

    goods_item_t g7 = {0};
    g7.id           = 1008;
    strcpy(g7.name, "Innogate Intel Core i7 with 18 cores, 4060 graphics card, 64G memory");
    strcpy(g7.price, "$ 4599");
    strcpy(g7.img_path, "S:/zj3.png");
    purchase_goods_list[purchase_goods_count++] = g7;

    goods_item_t g8 = {0};
    g8.id           = 1009;
    strcpy(g8.name, "ASUS ROG Nightstalker All-in-One 9800X3D R7 9700X + RTX 5060TI");
    strcpy(g8.price, "$ 7999");
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

// ========== 购买页面动态渲染 ==========
void refresh_purchase_page()
{
    if(tab2 == NULL || !lv_obj_is_valid(tab2) || tab2_1 == NULL || !lv_obj_is_valid(tab2_1) || tab2_2 == NULL ||
       !lv_obj_is_valid(tab2_2) || tab2_3 == NULL || !lv_obj_is_valid(tab2_3)) {
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
