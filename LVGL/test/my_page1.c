#include "myhead.h"

// 通用输入框焦点/失焦事件回调：实现软键盘复用+失焦隐藏
// 通用输入框焦点回调：用删除/重建替代显示/隐藏
void ta_focus_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta        = lv_event_get_target(e);

    if(code == LV_EVENT_FOCUSED) {
        // 先删除旧的软键盘（如果存在）
        if(kb != NULL) {
            lv_obj_del(kb);
        }
        // 重新创建并关联当前输入框
        kb = lv_keyboard_create(page1);
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

// 空白处点击回调：同样用删除替代隐藏
void page1_click_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    if(obj == page1 && kb != NULL) {
        lv_obj_del(kb);
        kb = NULL;
    }
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

// 登录页面1
void show_login_page1()
{
    
    page1 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(page1, 800, 480);
    lv_obj_set_pos(page1, 0, 0);

    // 给登录界面添加点击事件：点击空白处隐藏软键盘
    lv_obj_add_event_cb(page1, page1_click_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * img = lv_img_create(page1);
    LV_IMG_DECLARE(image1);
    lv_img_set_src(img, &image1);
    lv_obj_align_to(img, page1, LV_ALIGN_CENTER, 0, 0);

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


    // 绑定事件
    lv_obj_add_event_cb(ta1, ta_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(ta1, ta_focus_cb, LV_EVENT_DEFOCUSED, NULL);
    lv_obj_add_event_cb(ta2, ta_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(ta2, ta_focus_cb, LV_EVENT_DEFOCUSED, NULL);
    lv_obj_add_event_cb(bt1, bt_login_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(bt2, bt_regt_cb, LV_EVENT_CLICKED, NULL);
}