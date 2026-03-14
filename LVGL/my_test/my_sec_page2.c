#include "myhead.h"

// ========== 定时器回调函数 ==========
void toast_timer_cb(lv_timer_t * timer)
{
    lv_obj_t * toast = (lv_obj_t *)timer->user_data;
    if(toast && lv_obj_is_valid(toast)) {
        lv_obj_del(toast);
    }
    lv_timer_del(timer);
}

// ========== 管理员页面商品数据层核心函数 ==========
// 1. 新增商品到管理员列表（0成功/-1满/-2ID重复）
int admin_add_goods(goods_item_t new_goods)
{
    if(current_goods_count >= MAX_GOODS_COUNT) return -1;
    // 检查ID重复
    for(int i = 0; i < current_goods_count; i++) {
        if(global_goods_list[i].id == new_goods.id) return -2;
    }
    goods_item_t * goods = &global_goods_list[current_goods_count];
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

    current_goods_count++;
    return 0;
}

// 2. 从管理员列表删除商品
int admin_delete_goods(int goods_id)
{
    if(current_goods_count <= 0 || current_goods_count > MAX_GOODS_COUNT) {
        printf("Invalid current_goods_count: %d\n", current_goods_count);
        return -1;
    }

    int found = -1;
    printf("111\n");
    for(int i = 0; i < current_goods_count; i++) {
        if(global_goods_list[i].id == goods_id) {
            found = i;
            printf("222  %d\n", i);
            break;
        }
        printf("333  %d\n", i);
    }

    if(found == -1) return -1;

    // 数组前移覆盖
    for(int i = found; i < current_goods_count - 1; i++) {

        global_goods_list[i] = global_goods_list[i + 1];
        printf("444  %d\n", i);
    }
    printf("555\n");
    memset(&global_goods_list[current_goods_count - 1], 0, sizeof(goods_item_t));
    printf("666\n");
    current_goods_count--;
    printf("777   %d\n", current_goods_count);

    return 0;
}

// 3. 修改管理员列表中的商品
int admin_update_goods(int goods_id, goods_item_t new_info)
{
    for(int i = 0; i < current_goods_count; i++) {
        if(global_goods_list[i].id == goods_id) {
            // 名称更新
            if(new_info.name[0] != '\0') {
                strncpy(global_goods_list[i].name, new_info.name, sizeof(global_goods_list[i].name) - 1);
                global_goods_list[i].name[sizeof(global_goods_list[i].name) - 1] = '\0';
            }
            // 价格更新
            if(new_info.price[0] != '\0') {
                strncpy(global_goods_list[i].price, new_info.price, sizeof(global_goods_list[i].price) - 1);
                global_goods_list[i].price[sizeof(global_goods_list[i].price) - 1] = '\0';
            }
            // 图片路径更新
            if(new_info.img_path[0] != '\0') {
                strncpy(global_goods_list[i].img_path, new_info.img_path, sizeof(global_goods_list[i].img_path) - 1);
                global_goods_list[i].img_path[sizeof(global_goods_list[i].img_path) - 1] = '\0';
            }
            return 0;
        }
    }
    return -1;
}

// 4. 从管理员列表查询商品
goods_item_t * admin_query_goods(int goods_id)
{
    for(int i = 0; i < current_goods_count; i++) {
        if(global_goods_list[i].id == goods_id) {
            return &global_goods_list[i];
        }
    }
    return NULL;
}

// ========== 管理员商品管理功能 ==========
// 刷新管理员商品列表展示
void refresh_admin_goods_list()
{
    if(admin_goods_list_label = NULL) return;

    // lv_obj_clean(admin_goods_list_label);

    char list_text[4096] = "Goods List:\n";

    for(int i = 0; i < current_goods_count; i++) {
        char item_text[200];
        snprintf(item_text, sizeof(item_text), "ID: %d | Name: %s | Price: %s\n", global_goods_list[i].id,
                 global_goods_list[i].name, global_goods_list[i].price);
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
        lv_label_set_text(toast, "Add goods success!");

        // 清空表单
        lv_textarea_set_text(admin_ta_id, "");
        lv_textarea_set_text(admin_ta_name, "");
        lv_textarea_set_text(admin_ta_price, "");
        lv_textarea_set_text(admin_ta_img, "");
        // 刷新列表和购买页面
        refresh_admin_goods_list();
        refresh_purchase_page();
    } else if(ret == -1) {
        lv_label_set_text(toast, "Goods list full!");
    } else if(ret == -2) {
        lv_label_set_text(toast, "ID already exists!");
    }
    lv_obj_align(toast, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_style_text_font(toast, &lv_font_montserrat_16, LV_STATE_DEFAULT);
    lv_timer_create(toast_timer_cb, 2000, toast);
}

int cart_delete_goods(int goods_id)
{
    // 问题1修复：全量边界校验，非法计数直接返回
    if(cart_item_count <= 0 || cart_item_count > MAX_CART_ITEMS) {
        printf("cart_delete_goods: 购物车计数非法！count=%d\n", cart_item_count);
        return -1;
    }

    int delete_num = 0; // 记录删除的数量
    int i = 0;
    // 遍历购物车（用while循环，适配删除后计数变化的场景）
    while(i < cart_item_count) {
        if(shopping_cart[i].goods.id == goods_id) {
            // 数组前移：循环条件正确，无越界（和商品删除一致）
            for(int j = i; j < cart_item_count - 1; j++) {
                shopping_cart[j] = shopping_cart[j + 1];
            }
            // 问题2修复：先校验计数，再清空最后一个元素（避免shopping_cart[-1]）
            if(cart_item_count - 1 >= 0) {
                memset(&shopping_cart[cart_item_count - 1], 0, sizeof(cart_item_t));
            }
            cart_item_count--; // 计数减1
            delete_num++;      // 删除数量加1
            // 若只需删除第一个匹配项，取消下面的注释，删除break即可
            // break;
        } else {
            i++; // 只有未删除时，i才自增（避免漏遍历）
        }
    }

    if(delete_num == 0) {
        printf("cart_delete_goods: 购物车中无商品ID=%d的项\n", goods_id);
        return -1;
    } else {
        printf("cart_delete_goods: 成功删除%d个商品ID=%d的购物车项\n", delete_num, goods_id);
        return 0;
    }
}

// 管理员删除商品回调
void admin_delete_goods_cb(lv_event_t * e)
{
    if(admin_ta_id == NULL || !lv_obj_is_valid(admin_ta_id)) {
        lv_obj_t * toast = lv_label_create(lv_scr_act());
        lv_label_set_text(toast, "Textarea object is invalid!");
        lv_obj_align(toast, LV_ALIGN_TOP_MID, 0, 20);
        lv_timer_create(toast_timer_cb, 2000, toast);
        return;
    }

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
    printf("%d\n", ret);
    lv_obj_t * toast = lv_label_create(lv_scr_act());
    lv_obj_align(toast, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_style_text_font(toast, &lv_font_montserrat_16, LV_STATE_DEFAULT);

    if(ret == 0) {
        lv_label_set_text(toast, "Delete success!");
        lv_textarea_set_text(admin_ta_id, "");

        refresh_admin_goods_list();

        refresh_purchase_page();
        // 同步刷新购物车（删除已下架商品）
        // for(int i = 0; i < cart_item_count; i++) {
        //     if(shopping_cart[i].goods.id == id) {
        //         for(int j = i; j < cart_item_count - 1; j++) {
        //             shopping_cart[j] = shopping_cart[j + 1];
        //         }
        //         memset(&shopping_cart[cart_item_count - 1], 0, sizeof(cart_item_t));
        //         cart_item_count--;
        //         break;
        //     }
        // }
        cart_delete_goods(id);
        // refresh_shopping_cart_page();
    } else {
        lv_label_set_text(toast, "Goods not found!");
    }

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
        refresh_purchase_page();
    } else {
        lv_label_set_text(toast, "Update admin list failed!");
    }
    lv_obj_align(toast, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_style_text_font(toast, &lv_font_montserrat_16, LV_STATE_DEFAULT);
    lv_timer_create(toast_timer_cb, 2000, toast);
}

void sec2_ta_focus_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta        = lv_event_get_target(e);

    if(code == LV_EVENT_FOCUSED) {
        // 先删除旧的软键盘（如果存在）
        if(kb != NULL) {
            lv_obj_del(kb);
        }
        // 重新创建并关联当前输入框
        kb = lv_keyboard_create(sec_page2);
        lv_obj_set_size(kb, 800, 200);
        lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, -300);
        lv_keyboard_set_textarea(kb, ta);
    } else if(code == LV_EVENT_DEFOCUSED) {
        // 失焦时直接删除软键盘
        if(kb != NULL) {
            lv_obj_del(kb);
            kb = NULL;
        }
    }
}

void sec_page2_click_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    if(obj == sec_page2 && kb != NULL) {
        lv_obj_del(kb);
        kb = NULL;
    }
}

void admin_quit_cb(lv_event_t * e)
{
    show_login_page2();
    lv_obj_del(sec_page2);
    refresh_purchase_page();
    refresh_shopping_cart_page();
}

void show_sec_page2()
{
    // ========== 商品管理模块 ==========
    // 商品管理标题
    sec_page2 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(sec_page2, 800, 800);

    // 给登录界面添加点击事件：点击空白处隐藏软键盘
    lv_obj_add_event_cb(sec_page2, sec_page2_click_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * admin_title = lv_label_create(sec_page2);
    lv_label_set_text(admin_title, "Goods Management");
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

    // 绑定事件
    lv_obj_add_event_cb(admin_ta_id, sec2_ta_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(admin_ta_id, sec2_ta_focus_cb, LV_EVENT_DEFOCUSED, NULL);
    lv_obj_add_event_cb(admin_ta_name, sec2_ta_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(admin_ta_name, sec2_ta_focus_cb, LV_EVENT_DEFOCUSED, NULL);
    lv_obj_add_event_cb(admin_ta_price, sec2_ta_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(admin_ta_price, sec2_ta_focus_cb, LV_EVENT_DEFOCUSED, NULL);
    lv_obj_add_event_cb(admin_ta_img, sec2_ta_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(admin_ta_img, sec2_ta_focus_cb, LV_EVENT_DEFOCUSED, NULL);
}
