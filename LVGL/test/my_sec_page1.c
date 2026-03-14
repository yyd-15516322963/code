#include "myhead.h"

void sec_page1_click_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    if(obj == sec_page1 && kb != NULL) {
        lv_obj_del(kb);
        kb = NULL;
    }
}


void sec1_ta_focus_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta        = lv_event_get_target(e);

    if(code == LV_EVENT_FOCUSED) {
        // 先删除旧的软键盘（如果存在）
        if(kb != NULL) {
            lv_obj_del(kb);
        }
        // 重新创建并关联当前输入框
        kb = lv_keyboard_create(sec_page1);
        lv_obj_set_size(kb, 800, 200);
        lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 15);
        lv_keyboard_set_textarea(kb, ta);
    } else if(code == LV_EVENT_DEFOCUSED) {
        // 失焦时直接删除软键盘
        if(kb != NULL) {
            lv_obj_del(kb);
            kb = NULL;
        }
    }
}
// 管理员登录回调
void bt_sec_login_cb(lv_event_t * e)
{
    const char * ad_name = lv_textarea_get_text(sec_ta1);
    const char * ad_pwd  = lv_textarea_get_text(sec_ta2);

    if(ad_name == NULL) ad_name = "";
    if(ad_pwd == NULL) ad_pwd = "";

    // 检查用户名或密码是否为空
    if(strlen(ad_name) == 0 || strlen(ad_pwd) == 0) {
        return;
    }

    if(strcmp(ad_name, "yyd") == 0 && strcmp(ad_pwd, "123456") == 0) {
        show_sec_page2();
        if(sec_page1 != NULL && lv_obj_is_valid(sec_page1)) {
            lv_obj_del(sec_page1);
        }
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

// 管理员消息盒子
void bt_sec_msgbox_cb(lv_event_t * e)
{
    const char * btns[] = {"", "", ""};
    lv_obj_t * sec_msgbox =
        lv_msgbox_create(sec_page1, "Warning", "The password/account you entered is incorrect.", btns, true);
    lv_obj_set_pos(sec_msgbox, 260, 80);
}

// 管理员页面
void show_sec_page1()
{
    sec_page1 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(sec_page1, 800, 480);

    // 给登录界面添加点击事件：点击空白处隐藏软键盘
    lv_obj_add_event_cb(sec_page1, sec_page1_click_cb, LV_EVENT_CLICKED, NULL);

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


    // 绑定事件
    lv_obj_add_event_cb(sec_ta1, sec1_ta_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(sec_ta1, sec1_ta_focus_cb, LV_EVENT_DEFOCUSED, NULL);
    lv_obj_add_event_cb(sec_ta2, sec1_ta_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(sec_ta2, sec1_ta_focus_cb, LV_EVENT_DEFOCUSED, NULL);
    lv_obj_add_event_cb(sec_bt1, bt_sec_login_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(sec_bt2, bt_sec_quit_cb, LV_EVENT_CLICKED, NULL);
}